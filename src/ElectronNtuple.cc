#include "LowPtElectrons/LowPtElectrons/interface/ElectronNtuple.h"
#include "TTree.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/ParticleFlowReco/interface/PreId.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

using namespace reco;
using namespace edm;
void ElectronNtuple::link_tree(TTree *tree) {
	tree->Branch("run",  &run_ , "run/i");
  tree->Branch("lumi", &lumi_, "lumi/i");
  tree->Branch("evt",  &evt_ , "evt/i");

	tree->Branch("is_e", &is_e_, "is_e/O");
	tree->Branch("is_e_not_matched", &is_e_not_matched_, "is_e_not_matched/O");
	tree->Branch("is_other", &is_other_, "is_other/O");

	tree->Branch("gen_pt" , &gen_pt_ , "gen_pt/f" );
	tree->Branch("gen_eta", &gen_eta_, "gen_eta/f");
	tree->Branch("gen_phi", &gen_phi_, "gen_phi/f");

	tree->Branch("trk_pt",				 	&trk_pt_				   , "trk_pt/f");
	tree->Branch("trk_eta",		 	  &trk_eta_		       , "trk_eta/f");
	tree->Branch("trk_phi",		 	  &trk_phi_     		 , "trk_phi/f");
	tree->Branch("trk_nhits",			&trk_nhits_        , "trk_nhits/f");
	tree->Branch("trk_high_purity",&trk_high_purity_	 , "trk_high_purity/i");
	tree->Branch("trk_dxy",			  &trk_dxy_		  		 , "trk_dxy/f");
	tree->Branch("trk_dxy_err",		&trk_dxy_err_			 , "trk_dxy_err/f");
	tree->Branch("trk_inp",  			&trk_inp_  	    	 , "trk_inp/f");
	tree->Branch("trk_outp",	  		&trk_outp_	  	   , "trk_outp/f");
	tree->Branch("trk_chi2red",    &trk_chi2red_      , "trk_chi2red/f"); 

	tree->Branch("preid_ibin", 					&preid_ibin_          , "preid_ibin/I");
	tree->Branch("preid_bdtout",				 	&preid_bdtout_		  	, "preid_bdtout/f");
	tree->Branch("preid_trk_ecal_Deta",	&preid_trk_ecal_Deta_ , "preid_trk_ecal_Deta/f");
	tree->Branch("preid_trk_ecal_Dphi",	&preid_trk_ecal_Dphi_ , "preid_trk_ecal_Dphi/f");
	tree->Branch("preid_e_over_p",			 	&preid_e_over_p_			, "preid_e_over_p/f");
	//stage 2, with GSF
	tree->Branch("preid_gsf_success"			, &preid_gsf_success_     , "preid_gsf_success/O");
	tree->Branch("preid_gsf_dpt"					, &preid_gsf_dpt_		  		, "preid_gsf_dpt/f");					
	tree->Branch("preid_trk_gsf_chiratio", &preid_trk_gsf_chiratio_, "preid_trk_gsf_chiratio/f");
	tree->Branch("preid_gsf_chi2red"     , &preid_gsf_chi2red_     , "preid_gsf_chi2red/f");     
	tree->Branch("preid_numGSF", &preid_numGSF_, "preid_gsf_chi2red/i");
	//step-wise standard selection
	tree->Branch("preid_trk_ecal_match", &preid_trk_ecal_match_, "preid_trk_ecal_match/O");
	tree->Branch("preid_trkfilter_pass", &preid_trkfilter_pass_, "preid_trkfilter_pass/O");
	tree->Branch("preid_mva_pass", &preid_mva_pass_, "preid_mva_pass/O");
	
	tree->Branch("gsf_pt",				 	&gsf_pt_				   , "gsf_pt/f");
	tree->Branch("gsf_eta",		 	  &gsf_eta_		       , "gsf_eta/f");
	tree->Branch("gsf_phi",		 	  &gsf_phi_     		 , "gsf_phi/f");
	tree->Branch("gsf_nhits",			&gsf_nhits_        , "gsf_nhits/f");
	tree->Branch("gsf_dxy",			  &gsf_dxy_		  		 , "gsf_dxy/f");
	tree->Branch("gsf_dxy_err",		&gsf_dxy_err_			 , "gsf_dxy_err/f");
	tree->Branch("gsf_inp",  			&gsf_inp_  	    	 , "gsf_inp/f");
	tree->Branch("gsf_outp",	  		&gsf_outp_	  	   , "gsf_outp/f");
	tree->Branch("gsf_chi2red",    &gsf_chi2red_      , "gsf_chi2red/f"); 

	//PFGSFTrack internal steps flags
	tree->Branch("pfgsf_gsf_has_ktf", &pfgsf_gsf_has_ktf_, "pfgsf_gsf_has_ktf/O");
	tree->Branch("pfgsf_ktf_is_fifthStep", &pfgsf_ktf_is_fifthStep_, "pfgsf_ktf_is_fifthStep/O");
	tree->Branch("pfgsf_gsf_ecalDriven", &pfgsf_gsf_ecalDriven_, "pfgsf_gsf_ecalDriven/O");
	tree->Branch("pfgsf_gsf_trackerDriven", &pfgsf_gsf_trackerDriven_, "pfgsf_gsf_trackerDriven/O");
	tree->Branch("pfgsf_valid_gsf_brem", &pfgsf_valid_gsf_brem_, "pfgsf_valid_gsf_brem/O");
	tree->Branch("pfgsf_passes_preselection", &pfgsf_passes_preselection_, "pfgsf_passes_preselection/O");
	tree->Branch("pfgsf_passes_selection", &pfgsf_passes_selection_, "pfgsf_passes_selection/O");

	tree->Branch("pfgsf_xclean_seedref", &pfgsf_xclean_seedref_, "pfgsf_xclean_seedref/O");
	tree->Branch("pfgsf_xclean_ECALDriven_too_few_hits", &pfgsf_xclean_ECALDriven_too_few_hits_, "pfgsf_xclean_ECALDriven_too_few_hits/O");
	tree->Branch("pfgsf_xclean_ECALDriven_bad_EoverP", &pfgsf_xclean_ECALDriven_bad_EoverP_, "pfgsf_xclean_ECALDriven_bad_EoverP/O");
	tree->Branch("pfgsf_xclean_TrkDriven_too_few_hits", &pfgsf_xclean_TrkDriven_too_few_hits_, "pfgsf_xclean_TrkDriven_too_few_hits/O");
	tree->Branch("pfgsf_xclean_TrkDriven_vs_ECALDriven", &pfgsf_xclean_TrkDriven_vs_ECALDriven_, "pfgsf_xclean_TrkDriven_vs_ECALDriven/O");
	tree->Branch("pfgsf_xclean_BothTrk_bad_EoverP", &pfgsf_xclean_BothTrk_bad_EoverP_, "pfgsf_xclean_BothTrk_bad_EoverP/O");
	tree->Branch("pfgsf_xclean_BothTrk_noECAL_match", &pfgsf_xclean_BothTrk_noECAL_match_, "pfgsf_xclean_BothTrk_noECAL_match/O");
	tree->Branch("pfgsf_xclean_AngularGsfCleaning", &pfgsf_xclean_AngularGsfCleaning_, "pfgsf_xclean_AngularGsfCleaning/O");
	tree->Branch("pfgsf_xclean_noECAL_match_AGAIN", &pfgsf_xclean_noECAL_match_AGAIN_, "pfgsf_xclean_noECAL_match_AGAIN/O");
	tree->Branch("pfgsf_xclean_FINAL", &pfgsf_xclean_FINAL_, "pfgsf_xclean_FINAL/O");

	//Middle steps
	tree->Branch("has_ele_core", &has_ele_core_, "has_ele_core/O");
	tree->Branch("has_pfEgamma", &has_pfEgamma_,  "has_pfEgamma/O");
	tree->Branch("has_pfGSF_trk", &has_pfGSF_trk_, "has_pfGSF_trk/O");
	tree->Branch("has_pfBlock_with_SC", &has_pfBlock_with_SC_, "has_pfBlock_with_SC/O");
	tree->Branch("has_pfBlock_with_ECAL", &has_pfBlock_with_ECAL_, "has_pfBlock_with_ECAL/O");
	tree->Branch("has_pfBlock", &has_pfBlock_, "has_pfBlock/O");

	//bool has_pfEgamma_ = false;

	tree->Branch("ele_pt",				 	&ele_pt_				   , "ele_pt/f");
	tree->Branch("ele_eta",		 	  &ele_eta_		       , "ele_eta/f");
	tree->Branch("ele_phi",		 	  &ele_phi_     		 , "ele_phi/f");
}

//fillers
void ElectronNtuple::fill_evt(const edm::EventID &id) {
	run_  = id.run();						
  lumi_ = id.luminosityBlock();
  evt_  = id.event();          
}

void ElectronNtuple::fill_gen(const GenParticleRef genp) {
	gen_pt_  = genp->pt();
	gen_eta_ = genp->eta();
	gen_phi_ = genp->phi();
}

void ElectronNtuple::fill_gsf_trk(const GsfTrackRef trk, const reco::BeamSpot &spot) {
  if ( trk.isNonnull() ) {
    // kine
    gsf_pt_ = trk->pt();
    gsf_eta_ = trk->eta();
    gsf_phi_ = trk->phi();
    gsf_inp_ = sqrt(trk->innerMomentum().mag2());
    gsf_outp_ = sqrt(trk->outerMomentum().mag2());
    gsf_dpt_ = ( gsf_inp_ > 0. ) ? fabs( gsf_outp_ - gsf_inp_ ) / gsf_inp_ : 0.; //@@ redundant?
    // quality
    gsf_nhits_ = trk->found();
    gsf_chi2red_ = trk->normalizedChi2();
    // displ
    gsf_dxy_ = trk->dxy(spot);
    gsf_dxy_err_ = trk->dxyError();
    gsf_dz_ = trk->dz(spot.position());
    gsf_dz_err_ = trk->dzError();
  } else {
    //@@ Shouldn't happen, but we take dummy values ...?
  }
}

void ElectronNtuple::fill_preid( const PreId &preid, const reco::BeamSpot &spot, const int num_gsf) {

  // Extract KF track parameters
  fill_ktf_trk( preid.trackRef(), spot );

  // ECAL/track matching parameters
  preid_e_over_p_ = preid.eopMatch();
  preid_trk_ecal_Deta_ = preid.geomMatching()[0];
  preid_trk_ecal_Dphi_ = preid.geomMatching()[1];

  // GSF tracks
  preid_gsf_success_ = false; //@@ ??
  // (p_out-p_in)/p_in from GSF track
  preid_gsf_dpt_ = preid.dpt();
  // Ratio of chi2 from GSF and KF tracks
  preid_trk_gsf_chiratio_ = preid.chi2Ratio();
  // Estimate of reduced chi2 for GSF track (assumes GSF and KF track have same d.o.f.)
  preid_gsf_chi2red_ = preid.gsfChi2();
	
  // MVA output
  preid_bdtout_ = preid.mva();
  preid_ibin_ = preid.ibin();

	//How many GSF it will seed
	preid_numGSF_ = num_gsf;

	//step-wise standard selection
  preid_trk_ecal_match_ = preid.ecalMatching();
	preid_trkfilter_pass_ = preid.trackFiltered();
	preid_mva_pass_ = preid.mvaSelected();
}

void ElectronNtuple::fill_ele(const GsfElectronRef ele) {
	ele_pt_			 = ele->pt();
	ele_eta_		 = ele->eta();
	ele_phi_     = ele->phi();
}

void ElectronNtuple::fill_ktf_trk( const TrackRef trk, const reco::BeamSpot &spot ) {
  if ( trk.isNonnull() ) {
    // kine
    trk_pt_ = trk->pt();
    trk_eta_ = trk->eta();
    trk_phi_ = trk->phi();
    trk_inp_ = sqrt( trk->innerMomentum().mag2() );
    trk_outp_ = sqrt( trk->outerMomentum().mag2() );
    trk_dpt_ = ( trk_inp_ > 0. ) ? fabs( trk_outp_ - trk_inp_ ) / trk_inp_ : 0.; //@@ redundant?
    // quality
    trk_nhits_ = trk->found();
    trk_high_purity_ = trk->quality( TrackBase::qualityByName("highPurity") );
    trk_chi2red_ = trk->normalizedChi2();
    // displ
    trk_dxy_ = trk->dxy(spot);
    trk_dxy_err_ = trk->dxyError();
    trk_dz_ = trk->dz(spot.position());
    trk_dz_err_ = trk->dzError();
  } else {
    //@@ Shouldn't happen, but we take dummy values ...?
  }
}

void ElectronNtuple::unpack_pfgsf_flags(const int flags) {
	pfgsf_gsf_has_ktf_       	= get_ith_bit(flags, 0);
	pfgsf_ktf_is_fifthStep_  	= get_ith_bit(flags, 1);
	pfgsf_gsf_ecalDriven_    	= get_ith_bit(flags, 2);
	pfgsf_gsf_trackerDriven_ 	= get_ith_bit(flags, 3);
	pfgsf_valid_gsf_brem_    	= get_ith_bit(flags, 4);
	pfgsf_passes_preselection_ = get_ith_bit(flags, 5);
	pfgsf_passes_selection_    = get_ith_bit(flags, 6);

	pfgsf_xclean_seedref_ = get_ith_bit(flags, 7);
	pfgsf_xclean_ECALDriven_too_few_hits_ = get_ith_bit(flags, 8);
	pfgsf_xclean_ECALDriven_bad_EoverP_ = get_ith_bit(flags, 9);
	pfgsf_xclean_TrkDriven_too_few_hits_ = get_ith_bit(flags, 10);
	pfgsf_xclean_TrkDriven_vs_ECALDriven_ = get_ith_bit(flags, 11);
	pfgsf_xclean_BothTrk_bad_EoverP_ = get_ith_bit(flags, 12);
	pfgsf_xclean_BothTrk_noECAL_match_ = get_ith_bit(flags, 13);
	pfgsf_xclean_AngularGsfCleaning_ = get_ith_bit(flags, 14);
	pfgsf_xclean_noECAL_match_AGAIN_ = get_ith_bit(flags, 15);
	pfgsf_xclean_FINAL_ = get_ith_bit(flags, 16);
}
