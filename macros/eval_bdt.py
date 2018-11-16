import numpy as np
import matplotlib
matplotlib.use('Agg')
from argparse import ArgumentParser
from cmsjson import CMSJson
from pdb import set_trace
import os
from glob import glob

parser = ArgumentParser()
parser.add_argument('model')
parser.add_argument('--what')
parser.add_argument('--dataset')
parser.add_argument('--plot')
parser.add_argument('--query')
parser.add_argument('--weights', help='train_weight:original_weight')
args = parser.parse_args()

def check_consistency(pars, jfile):
    pars['max_depth'] = int(pars['max_depth'])
    if not os.path.isfile(jfile):
        return False
    jpars = json.load(open(jfile))
    if 'n_estimators' in jpars: del jpars['n_estimators']
    if 'auc' in jpars: del jpars['auc']
    if set(jpars.keys()) != set(pars.keys()):
        raise ValueError('The hyperparameter keys do not match!')
    return all(abs(jpars[hyper]-pars[hyper])/(pars[hyper] if pars[hyper] else 1) < 0.001 for hyper in jpars)

def file_recovery(pars, dname):
    parfiles = glob('%s/pars_*.json' % base)
    for jfile in parfiles:
        if check_consistency(pars, jfile):
            break
    else:
        return None
    return jfile.replace('pars_', 'model_').replace('.pkl', '.json')

import pandas as pd
import json
from pprint import pprint
import matplotlib.pyplot as plt
if args.model.endswith('.csv'):    
    bo = pd.read_csv(args.model)
    best = bo.target.argmax()
    print 'best hyperparameters location: ', best
    pprint(dict(bo.iloc[best])) 
    pars = dict(bo.loc[best])
    del pars['target']
    base = os.path.dirname(args.model)
    
    args.what = base.split('bdt_bo_')[1].replace('_noweight','')
    model = '%s/model_%d.pkl' % (base, best)
    jfile = '%s/pars_%d.json' % (base, best)
    if not check_consistency(pars, jfile):
        print 'something went wrong, trying to recover...'
        model = file_recovery(pars, base)
        if model is None:
            rescue = '%s/rescue_%d.json' % (base, best)
            with open('%s/rescue_%d.json' % (base, best), 'w') as rsc:
                json.dump(pars, rsc)
            raise RuntimeError(
                'Something went horribly wrong and could not be recovered'
                ', stored the rescue hyperparameter setup in %s' % rescue
                )
        
    print 'using model', model
    dataset = glob('%s/*.hdf' % base)[0]
    plots = base if not args.plot else args.plot
else:
    model = args.model
    dataset = args.dataset
    plots = os.dirname(model) if not args.plot else args.plot
    if not dataset:
        raise RuntimeError('You must specify a dataset if you are not running in Bayesian Optimization mode')

#this should be outsorced
from features import *
features, additional = get_features(args.what)

## Fix for the old code
## features = [
##    'trk_pt',
##    'trk_eta',
##    'trk_phi',
##    'trk_p',
##    'trk_charge',
##    'trk_nhits',
##    'trk_high_purity',
##    'trk_inp',
##    'trk_outp',
##    'trk_chi2red',
##    'preid_trk_ecal_Deta',
##    'preid_trk_ecal_Dphi',
##    'preid_e_over_p',
## ]

from scipy.interpolate import InterpolatedUnivariateSpline
def bootstrapped_roc(y_true, y_pred, sample_weight=None, n_boots=200):
   'from https://stackoverflow.com/questions/19124239/scikit-learn-roc-curve-with-confidence-intervals'
   newx = np.logspace(-4, 0, 100)
   tprs = pd.DataFrame()
   if sample_weight.min() == sample_weight.max() and sample_weight.min() == 1.:
       sample_weight = None

   for iboot in range(n_boots):
      idxs = np.random.random_integers(0, y_pred.shape[0] - 1, y_pred.shape[0])
      try:
          fakes, effs, _ = roc_curve(
              y_true[idxs], y_pred[idxs], 
              sample_weight=None if sample_weight is None else sample_weight[idxs]
              )
      except RuntimeError:
          set_trace()
          print 'ROC Computation failed'
          continue

      #remove duplicates in the ROC
      coords = pd.DataFrame()
      coords['fpr'] = fakes
      coords['tpr'] = effs
      clean = coords.drop_duplicates(subset=['fpr'])
      #fit with a spline
      spline = InterpolatedUnivariateSpline(clean.fpr, clean.tpr,k=1)
      #make uniform spacing to allow averaging
      tprs[iboot] = spline(newx)
   return newx, tprs.mean(axis=1), tprs.std(axis=1)

from sklearn.externals import joblib
import xgboost as xgb
xml = model.replace('.pkl', '.xml')
model = joblib.load(model)
from datasets import HistWeighter

from xgbo.xgboost2tmva import convert_model
from itertools import cycle

# xgb sklearn API assigns default names to the variables, use that to dump the XML
# then convert them to the proper name
## xgb_feats = ['f%d' % i for i in range(len(features))]
## convert_model(model._Booster.get_dump(), zip(xgb_feats, cycle('F')), xml)
## xml_str = open(xml).read()
## for idx, feat in reversed(list(enumerate(features))):
##     xml_str = xml_str.replace('f%d' % idx, feat)
## with open(xml.replace('.xml', '.fixed.xml'), 'w') as XML:
##     XML.write(xml_str)

def _monkey_patch():
    return model._Booster

if isinstance(model.booster, basestring):
    model.booster = _monkey_patch

test = pd.read_hdf(dataset, key='data') \
   if ':' not in dataset else \
   pd.read_hdf(dataset.split(':')[0], key=dataset.split(':')[1])

if args.query:
    test = test.query(args.query)

if 'original_weight' not in test.columns:
    test['original_weight'] = 1.
else:
    original_weight = HistWeighter('../data/fakesWeights.txt')
    test['original_weight'] = np.invert(test.is_e)*original_weight.get_weight(test.log_trkpt, test.trk_eta)+test.is_e

if args.weights:
    weight, orig = tuple(args.weights.split(':'))
    test['weight'] = test[weight] if weight else 1.
    test['original_weight'] = test[orig] if orig else 1.
    test.baseline = test.baseline.astype(bool)

#
# plot performance
#
from sklearn.metrics import roc_curve, roc_auc_score
from scipy.special import expit
training_out = model.predict_proba(test[features].as_matrix())[:,1]
training_out[np.isnan(training_out)] = -999 #happens rarely, but happens

# Working points
info = roc_curve(
    test.is_e, training_out, 
    sample_weight=test.weight
    )

with open('%s/wp.json' % plots, 'w') as rr:
    rr.write('eff \tfr  \tthr\n')
    for eff in [0.95, 0.9, 0.8]:
        idx = np.abs(info[1] - eff).argmin()
        rr.write('%.2f\t%.2f\t%.2f\n' % (info[1][idx], info[0][idx], info[2][idx]))

#weighted ROC (same as training)
roc = bootstrapped_roc(
   test.is_e.as_matrix().astype(int), 
   training_out,
   sample_weight=test.weight.as_matrix())
auc_score = roc_auc_score(test.is_e, training_out, 
                          sample_weight=test.weight)

plt.figure(figsize=[8, 8])
plt.title('%s training' % args.what)
plt.plot(
   np.arange(0,1,0.01),
   np.arange(0,1,0.01),
   'k--')
plt.plot(roc[0], roc[1], 'b-', label='Retraining (AUC: %.2f)'  % auc_score)
plt.fill_between(roc[0], roc[1]-roc[2], roc[1]+roc[2], color='b', alpha=0.3)
plt.xlabel('Mistag Rate')
plt.ylabel('Efficiency')
plt.legend(loc='best')
plt.xlim(0., 1)
plt.savefig('%s/weighted_ROC.png' % (plots))
plt.savefig('%s/weighted_ROC.pdf' % (plots))
plt.gca().set_xscale('log')
plt.xlim(1e-4, 1)
plt.savefig('%s/weighted_ROC_log.png' % (plots))
plt.savefig('%s/weighted_ROC_log.pdf' % (plots))
plt.clf()


# ROC as in data
roc = bootstrapped_roc(
   test.is_e.as_matrix().astype(int), 
   training_out,
   sample_weight=test.original_weight.as_matrix())
auc_score = roc_auc_score(test.is_e, training_out, 
                          sample_weight=test.original_weight)


jmap = {
    'roc' : [
        list(roc[0]),
        list(roc[1])
        ],
    'auc' : auc_score,
}

# make plots
plt.figure(figsize=[8, 8])
plt.title('%s training' % args.what)
plt.plot(
   np.arange(0,1,0.01),
   np.arange(0,1,0.01),
   'k--')
plt.plot(roc[0], roc[1], label='Retraining (AUC: %.2f)'  % auc_score)
plt.fill_between(roc[0], roc[1]-roc[2], roc[1]+roc[2], color='b', alpha=0.3)
if 'seeding' in args.what and 'baseline' in test.columns:    
    eff = test.original_weight[(test.baseline & test.is_e)].sum()/test.original_weight[test.is_e].sum()
    mistag = test.original_weight[(test.baseline & np.invert(test.is_e))].sum()/test.original_weight[np.invert(test.is_e)].sum()
    jmap['baseline_mistag'] = mistag
    jmap['baseline_eff'] = eff
    plt.plot([mistag], [eff], 'o', label='baseline', markersize=5)
elif 'id' in args.what:
   mva_v1 = roc_curve(test.is_e, test.ele_mvaIdV1, sample_weight=test.original_weight)[:2]   
   mva_v2 = roc_curve(test.is_e, test.ele_mvaIdV2, sample_weight=test.original_weight)[:2]
   mva_v1_auc = roc_auc_score(test.is_e, test.ele_mvaIdV1, sample_weight=test.original_weight)
   mva_v2_auc = roc_auc_score(test.is_e, test.ele_mvaIdV2, sample_weight=test.original_weight)
   plt.plot(*mva_v1, label='MVA ID V1 (AUC: %.2f)'  % mva_v1_auc)
   plt.plot(*mva_v2, label='MVA ID V2 (AUC: %.2f)'  % mva_v2_auc)

plt.xlabel('Mistag Rate')
plt.ylabel('Efficiency')
plt.legend(loc='best')
plt.xlim(0., 1)
plt.savefig('%s/test_NN.png' % (plots))
plt.savefig('%s/test_NN.pdf' % (plots))
plt.gca().set_xscale('log')
plt.xlim(1e-4, 1)
plt.savefig('%s/test_log_NN.png' % (plots))
plt.savefig('%s/test_log_NN.pdf' % (plots))
plt.clf()

#ROCs by pT
plt.figure(figsize=[8, 8])
plt.title('%s training' % args.what)
plt.plot(
   np.arange(0,1,0.01),
   np.arange(0,1,0.01),
   'k--')

for low, high, col in [(0.,1., 'b'),
                  (1.,2., 'g'),
                  (2.,5., 'r'),
                  (5.,10., 'c'),
                  (10.,15., 'm')]:
    mask = (low <= test.trk_pt) & (test.trk_pt < high)
    masked = test[mask]
    roc = roc_curve(
        masked.is_e.as_matrix().astype(int), 
        training_out[mask],
        sample_weight=test.original_weight)[:2]
    auc_score = roc_auc_score(masked.is_e, training_out[mask], sample_weight=test.original_weight)
    plt.plot(*roc, c=col, label='p_T [%.2f, %.2f) (AUC: %.2f)'  % (low, high, auc_score))
    key = 'trk_pt#%.2fto%.2f' % (low, high)
    jmap[key] = {
        'roc' : [
            list(roc[0]),
            list(roc[1])
            ],
        'auc' : auc_score,
        }
    if 'seeding' in args.what:
        eff = masked.original_weight[(masked.baseline & masked.is_e)].sum()/masked.original_weight[masked.is_e].sum()
        mistag = masked.original_weight[(masked.baseline & np.invert(masked.is_e))].sum()/masked.original_weight[np.invert(masked.is_e)].sum()
        plt.plot([mistag], [eff], 'o', markersize=5, c=col)
        jmap[key]['baseline_eff'] = eff
        jmap[key]['baseline_mistag'] = mistag

plt.xlabel('Mistag Rate')
plt.ylabel('Efficiency')
plt.legend(loc='best')
plt.xlim(0., 1)
plt.savefig('%s/test_bypt_BDT.png' % (plots))
plt.savefig('%s/test_bypt_BDT.pdf' % (plots))
plt.gca().set_xscale('log')
plt.xlim(1e-4, 1)
plt.savefig('%s/test_log_bypt_BDT.png' % (plots))
plt.savefig('%s/test_log_bypt_BDT.pdf' % (plots))
plt.clf()

with open('%s/roc.json' % plots, 'w') as rr:
    rr.write(json.dumps(jmap))

importances = zip(features, model.feature_importances_)
importances.sort(key=lambda x: -x[1])
with open('%s/importances.txt' % plots, 'w') as rr:
    for name, val in importances:
        rr.write('%s\t\t%.5f\n' % (name, val))
