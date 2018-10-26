import numpy as np
import matplotlib
matplotlib.use('Agg')
from argparse import ArgumentParser
from cmsjson import CMSJson
from pdb import set_trace

parser = ArgumentParser()
parser.add_argument('what')
parser.add_argument(
   '--jobtag', default='', type=str
)

parser.add_argument(
   '--test', action='store_true'
)

parser.add_argument(
   '--noweight', action='store_true'
)

args = parser.parse_args()

import matplotlib.pyplot as plt
import uproot
import json
import pandas as pd
from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
rc('text', usetex=True)
from datasets import tag, pre_process_data, get_models_dir, target_dataset
import os

dataset = 'test' if args.test else target_dataset
mods = get_models_dir()

opti_dir = '%s/bdt_bo_%s' % (mods, args.what)
if args.noweight:
   opti_dir += '_noweight'
if not os.path.isdir(opti_dir):
   os.makedirs(opti_dir)

plots = '%s/src/LowPtElectrons/LowPtElectrons/macros/plots/%s/' % (os.environ['CMSSW_BASE'], tag)
if not os.path.isdir(plots):
   os.makedirs(plots)

from features import *
features, additional = get_features(args.what)

fields = features+labeling+additional
if 'gsf_pt' not in fields : fields += ['gsf_pt']
data = pre_process_data(dataset, fields, 'seeding' in args.what)
if args.noweight:
   data.weight = 1

from sklearn.model_selection import train_test_split
train, test = train_test_split(data, test_size=0.2, random_state=42)
test.to_hdf(
   '%s/nn_bo_%s_testdata.hdf' % (opti_dir, args.what),
   'data'
   ) 

train, validation = train_test_split(train, test_size=0.2, random_state=42)

import xgboost as xgb
from sklearn.externals import joblib
from sklearn.metrics import roc_curve, roc_auc_score
iteration_idx = 0
def train_model(**kwargs):
   global iteration_idx
   print iteration_idx
   #sanitize inputs
   kwargs['max_depth'] = int(kwargs['max_depth'])
   kwargs['n_estimators'] = 5000

   clf = xgb.XGBClassifier(
      objective='binary:logitraw',
      **kwargs
      )

   early_stop_kwargs = {
      'eval_set' : [(validation[features].as_matrix(), validation.is_e.as_matrix().astype(int))],
      #'sample_weight_eval_set' : [test.weight.as_matrix()], #undefined in this version
      'eval_metric' : 'auc',
      'early_stopping_rounds' : 10
      } 

   clf.fit(
      train[features].as_matrix(), 
      train.is_e.as_matrix().astype(int), 
      sample_weight=train.weight.as_matrix(),
      **early_stop_kwargs
      )

   training_out = clf.predict_proba(validation[features].as_matrix())[:, 1]
   auc = roc_auc_score(validation.is_e, training_out)
   kwargs['auc'] = auc
   
   with open('%s/pars_%d.json' % (opti_dir, iteration_idx), 'w') as jpars:
      json.dump(kwargs, jpars)
   
   joblib.dump(
      clf, '%s/model_%d.pkl' % (opti_dir, iteration_idx), 
      compress=True
      )
   iteration_idx += 1
   return auc

#
# Bayesian optimization
#
from xgbo import BayesianOptimization
par_space = {
   'min_child_weight': (1, 20),
   'colsample_bytree': (0.1, 1),
   'max_depth': (2, 15),
   'subsample': (0.5, 1),
   'gamma': (0, 10),
   'reg_alpha': (0, 10),
   'reg_lambda': (0, 10),
   'learning_rate' : (0.001, 0.1),
   }

bo = BayesianOptimization(
   train_model,
   par_space,
   verbose=1,
   checkpoints='%s/checkpoints.csv' % opti_dir
)
bo.init(5, sampling='lhs')
bo.maximize(5, 50)

bo.print_summary()
with open('%s/bdt_bo.json' % opti_dir, 'w') as j:
    mpoint = bo.space.max_point()
    thash = mpoint['max_params'].__repr__().__hash__()
    info = mpoint['max_params']
    info['value'] = mpoint['max_val']
    info['hash'] = thash
    j.write(json.dumps(info))

