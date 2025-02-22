#ifndef LowPtElectrons_LowPtElectrons_IDNtuple
#define LowPtElectrons_LowPtElectrons_IDNtuple

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PreId.h"
#include "DataFormats/ParticleFlowReco/interface/PreIdFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "LowPtElectrons/LowPtElectrons/interface/Common.h"
#include <vector>

class TTree;

constexpr size_t ARRAY_SIZE = 20;

// Small class to provide fillers and hide tree I/O
class IDNtuple {

 public:
  
  IDNtuple() {}

  void reset() {
    IDNtuple dummy; // create a new object 
    *this = dummy; // use assignment to reset
  }
  
  void link_tree( TTree* tree );
  
  void set_weight( float w ) { weight_ = w; }
  void set_prescale( float p ) { prescale_ = p; }
  void set_rho( float r ) { rho_ = r; }

  void is_aod( int aod ) { is_aod_ = aod; }
  void is_mc( int mc ) { is_mc_ = mc; }

  void tag_pt( float x ) { tag_pt_ = x; }
  void tag_eta( float x ) { tag_eta_ = x; }

  void is_e( bool t = true ) { is_e_ = t; }
  void is_e_not_matched( bool t = true ) { is_e_not_matched_ = t; }
  void is_other( bool t = true ) { is_other_ = t; }
  void is_egamma( bool t = true ) { is_egamma_ = t; }

  void has_trk( bool f = false ) { has_trk_ = f; }
  void has_seed( bool f = false ) { has_seed_ = f; }
  void has_gsf( bool f = false ) { has_gsf_ = f; }
  void has_pfgsf( bool f = false ) { has_pfgsf_ = f; }
  void has_ele( bool f = false ) { has_ele_ = f; }

  void trk_dr( float dr ) { trk_dr_ = dr; }
  void gsf_dr( float dr ) { gsf_dr_ = dr; }
  void pfgsf_dr( float dr ) { pfgsf_dr_ = dr; }
  void ele_dr( float dr ) { ele_dr_ = dr; }

  void fill_evt( const edm::EventID& id );

  void fill_gen( const pat::PackedGenParticleRef );
  void fill_gen( const reco::GenParticlePtr ); //@@ AOD
  void fill_gen( const reco::CandidatePtr );

  void fill_trk( const reco::TrackPtr& trk,
		 const reco::BeamSpot& spot );

  void pdg_id( int id ) { pdg_id_ = id; }

  void fill_seed( bool seed_trk_driven, 
		  bool seed_ecal_driven );

   // to be deprecated
  void fill_seed( double seed_unbiased,
		  double seed_ptbiased );

  void fill_bdt( double seed_unbiased,
		 double seed_ptbiased );
  
  void fill_preid( const reco::PreId& preid_ecal,
		   const reco::PreId& preid_hcal,
		   const reco::BeamSpot& spot, 
		   const double rho, 
		   noZS::EcalClusterLazyTools& ecalTools );

  void fill_gsf( const reco::GsfTrackPtr gsf,
		 const reco::BeamSpot& spot );

  void fill_pfgsf( const reco::GsfTrackPtr pfgsf,
		 const reco::BeamSpot& spot );

  void fill_ele( const reco::GsfElectronPtr ele,
		 float mva_value_pf,
		 float mva_value_pf_retrained, 
		 float mva_value_2019Aug07,
		 float mva_value_depth10_2020Sept15,
		 float mva_value_depth11_2020Nov28,
		 float mva_value_depth13_2021May17,
		 float mva_value_depth15_unknown,
		 float ele_conv_vtx_fit_prob,
		 const double rho,
		 bool is_egamma = false,
		 float seed_unbiased = -10. );
  
  void fill_image( const float gsf_ref_eta, const float gsf_ref_phi, const float gsf_ref_R,
		   const float gsf_ref_p, const float gsf_ref_pt,
		   const float gen_inner_eta, const float gen_inner_phi, const float gen_inner_R,
		   const float gen_inner_p, const float gen_inner_pt,
		   const float gen_proj_eta, const float gen_proj_phi, const float gen_proj_R,
		   const float gsf_inner_eta, const float gsf_inner_phi, const float gsf_inner_R,
		   const float gsf_inner_p, const float gsf_inner_pt, const int gsf_charge,
		   const float gsf_proj_eta, const float gsf_proj_phi, const float gsf_proj_R,
		   const float gsf_proj_p,
		   const float gsf_atcalo_eta, const float gsf_atcalo_phi, const float gsf_atcalo_R,
		   const float gsf_atcalo_p,
		   const std::vector<float>& clu_eta,
		   const std::vector<float>& clu_phi,
		   const std::vector<float>& clu_e,
		   const std::vector<int>& clu_nhit,
		   const std::vector<float>& pf_eta,
		   const std::vector<float>& pf_phi,
		   const std::vector<float>& pf_p,
		   const std::vector<int>& pf_pdgid,
		   const std::vector<int>& pf_matched,
		   const std::vector<int>& pf_lost );
  
 public:

  // Event
  unsigned int run_ = 0;
  unsigned int lumi_ = 0;
  unsigned long long evt_ = 0;
  float prescale_ = 0.;
  float weight_ = 1.;
  float rho_ = id::NEG_FLOAT;

  // Data sample
  int is_aod_ = -1;
  int is_mc_ = -1;

  // Tag-side muon
  float tag_pt_ = id::NEG_FLOAT;
  float tag_eta_ = id::NEG_FLOAT;

  // Labels
  bool is_e_ = false;
  bool is_e_not_matched_ = false;
  bool is_other_ = false;
  bool is_egamma_ = false;

  // RECO steps
  bool has_trk_ = false;
  bool has_seed_ = false;
  bool has_gsf_ = false;
  bool has_pfgsf_ = false;
  bool has_ele_ = false;

  float trk_dr_ = id::NEG_FLOAT;
  float gsf_dr_ = id::NEG_FLOAT;
  float pfgsf_dr_ = id::NEG_FLOAT;
  float ele_dr_ = id::NEG_FLOAT;

  // GEN electrons
  float gen_pt_ = id::NEG_FLOAT;
  float gen_eta_ = id::NEG_FLOAT;
  float gen_phi_ = id::NEG_FLOAT;
  float gen_e_ = id::NEG_FLOAT;
  float gen_p_ = id::NEG_FLOAT;
  int gen_charge_ = id::NEG_INT;
  int gen_pdgid_ = 0;
  int gen_mom_pdgid_ = 0;
  int gen_gran_pdgid_ = 0;

  // KF tracks: kine
  float trk_pt_ = id::NEG_FLOAT;
  float trk_eta_ = id::NEG_FLOAT;
  float trk_phi_ = id::NEG_FLOAT;
  float trk_p_ = id::NEG_INT;
  int trk_charge_ = 0; //@@ id::NEG_INT;
  float trk_inp_ = id::NEG_FLOAT;
  float trk_outp_ = id::NEG_FLOAT;
  float trk_dpt_ = id::NEG_FLOAT;

  int pdg_id_ = 0;

  // KF tracks: quality
  int trk_nhits_ = id::NEG_INT;
  int trk_missing_inner_hits_ = id::NEG_INT;
  int trk_high_purity_ = 0; //@@ id::NEG_INT;
  float trk_chi2red_ = id::NEG_FLOAT;

  // KF tracks: displ
  float trk_dxy_ = id::NEG_FLOAT;
  float trk_dxy_err_ = id::NEG_FLOAT;
  float trk_dz_ = id::NEG_FLOAT;
  float trk_dz_err_ = id::NEG_FLOAT;
  
  // Seed BDT discriminator values
  float preid_unbiased_ = id::NEG_FLOAT;
  float preid_ptbiased_ = id::NEG_FLOAT;

  float preid_trk_pt_ = id::NEG_FLOAT;
  float preid_trk_eta_ = id::NEG_FLOAT;
  float preid_trk_phi_ = id::NEG_FLOAT;
  float preid_trk_p_ = id::NEG_FLOAT;
  float preid_trk_nhits_ = id::NEG_FLOAT;
  float preid_trk_high_quality_ = id::NEG_FLOAT;
  float preid_trk_chi2red_ = id::NEG_FLOAT;
  float preid_rho_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_e_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_deta_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_dphi_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_e3x3_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_e5x5_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_covEtaEta_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_covEtaPhi_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_covPhiPhi_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_r9_ = id::NEG_FLOAT;
  float preid_ktf_ecal_cluster_circularity_ = id::NEG_FLOAT;
  float preid_ktf_hcal_cluster_e_ = id::NEG_FLOAT;
  float preid_ktf_hcal_cluster_deta_ = id::NEG_FLOAT;
  float preid_ktf_hcal_cluster_dphi_ = id::NEG_FLOAT;
  float preid_gsf_dpt_ = id::NEG_FLOAT;
  float preid_trk_gsf_chiratio_ = id::NEG_FLOAT;
  float preid_gsf_chi2red_ = id::NEG_FLOAT;
  float preid_trk_dxy_sig_ = id::NEG_FLOAT;

  // Seed BDT discriminator values at GsfTrack level
  float seed_unbiased_ = id::NEG_FLOAT;
  float seed_ptbiased_ = id::NEG_FLOAT;

  bool seed_trk_driven_ = false;
  bool seed_ecal_driven_ = false;

  // GSF tracks: kine
  float gsf_pt_ = id::NEG_FLOAT;
  float gsf_eta_ = id::NEG_FLOAT;
  float gsf_phi_ = id::NEG_FLOAT;
  float gsf_p_ = id::NEG_FLOAT;
  int gsf_charge_ = 0; //@@ id::NEG_INT;
  float gsf_inp_ = id::NEG_FLOAT;
  float gsf_outp_ = id::NEG_FLOAT;
  float gsf_dpt_ = id::NEG_FLOAT;

  // GSF tracks: kine (mode)
  float gsf_mode_pt_ = id::NEG_FLOAT;
  float gsf_mode_eta_ = id::NEG_FLOAT;
  float gsf_mode_phi_ = id::NEG_FLOAT;
  float gsf_mode_p_ = id::NEG_FLOAT;

  // GSF tracks: quality
  int gsf_nhits_ = id::NEG_INT;
  int gsf_missing_inner_hits_ = id::NEG_INT;
  float gsf_chi2red_ = id::NEG_FLOAT;

  // GSF tracks: displacement
  float gsf_dxy_ = id::NEG_FLOAT;
  float gsf_dxy_err_ = id::NEG_FLOAT;
  float gsf_dz_ = id::NEG_FLOAT;
  float gsf_dz_err_ = id::NEG_FLOAT;

  // GSF tracks: tangents
  int gsf_ntangents_ = 0; //@@ id::NEG_INT;
  float gsf_hit_dpt_[id::NHITS_MAX] = {0}; //@@ {id::NEG_FLOAT};
  float gsf_hit_dpt_unc_[id::NHITS_MAX] = {0}; //@@ {id::NEG_FLOAT};
  //std::vector<float> gsf_extapolated_eta_;
  //std::vector<float> gsf_extapolated_phi_;

  // PF GSF tracks: kine
  float pfgsf_pt_ = id::NEG_FLOAT;
  float pfgsf_eta_ = id::NEG_FLOAT;
  float pfgsf_phi_ = id::NEG_FLOAT;
  float pfgsf_p_ = id::NEG_FLOAT;
  int pfgsf_charge_ = 0; //@@ id::NEG_INT;
  float pfgsf_inp_ = id::NEG_FLOAT;
  float pfgsf_outp_ = id::NEG_FLOAT;
  float pfgsf_dpt_ = id::NEG_FLOAT;

  // PF GSF tracks: kine (mode)
  float pfgsf_mode_pt_ = id::NEG_FLOAT;
  float pfgsf_mode_eta_ = id::NEG_FLOAT;
  float pfgsf_mode_phi_ = id::NEG_FLOAT;
  float pfgsf_mode_p_ = id::NEG_FLOAT;

  // PF GSF tracks: quality
  int pfgsf_nhits_ = id::NEG_INT;
  int pfgsf_missing_inner_hits_ = id::NEG_INT;
  float pfgsf_chi2red_ = id::NEG_FLOAT;

  // PF GSF tracks: displacement
  float pfgsf_dxy_ = id::NEG_FLOAT;
  float pfgsf_dxy_err_ = id::NEG_FLOAT;
  float pfgsf_dz_ = id::NEG_FLOAT;
  float pfgsf_dz_err_ = id::NEG_FLOAT;

  // PF GSF tracks: tangents
  int pfgsf_ntangents_ = 0; //@@ id::NEG_INT;
  float pfgsf_hit_dpt_[id::NHITS_MAX] = {0}; //@@ {id::NEG_FLOAT};
  float pfgsf_hit_dpt_unc_[id::NHITS_MAX] = {0}; //@@ {id::NEG_FLOAT};
  //std::vector<float> pfgsf_extapolated_eta_;
  //std::vector<float> pfgsf_extapolated_phi_;

  // GSF electrons: kinematics
  float ele_pt_ = id::NEG_FLOAT;
  float ele_eta_ = id::NEG_FLOAT;
  float ele_phi_ = id::NEG_FLOAT;
  float ele_p_ = id::NEG_FLOAT;

  // Electrons: IDs
  float ele_mva_value_PF_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_PF_retrained_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_2019Aug07_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_2020Sept15_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_2020Nov28_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_2021May17_ = -999.; //@ id::NEG_FLOAT;
  float ele_mva_value_unknown_ = -999.; //@ id::NEG_FLOAT;
  float ele_conv_vtx_fit_prob_ = id::NEG_FLOAT;

  // Electrons: MVA variables for 2019Aug07
  float eid_trk_p_ = -666; //@@ id::NEG_FLOAT;
  float eid_trk_nhits_ = -666; //@@ id::NEG_FLOAT;
  float eid_trk_chi2red_ = -666; //@@ id::NEG_FLOAT;

  float eid_gsf_nhits_ = -666; //@@ id::NEG_FLOAT;
  float eid_gsf_chi2red_ = -666; //@@ id::NEG_FLOAT;

  float eid_sc_E_ = -666; //@@ id::NEG_FLOAT;
  float eid_sc_eta_ = -666; //@@ id::NEG_FLOAT;
  float eid_sc_etaWidth_ = -666; //@@ id::NEG_FLOAT;
  float eid_sc_phiWidth_ = -666; //@@ id::NEG_FLOAT;

  float eid_match_seed_dEta_ = -666; //@@ id::NEG_FLOAT;
  float eid_match_eclu_EoverP_ = -666; //@@ id::NEG_FLOAT;
  float eid_match_SC_EoverP_ = -666; //@@ id::NEG_FLOAT;
  float eid_match_SC_dEta_ = -666; //@@ id::NEG_FLOAT;
  float eid_match_SC_dPhi_ = -666; //@@ id::NEG_FLOAT;

  float eid_shape_full5x5_sigmaIetaIeta_ = -666; //@@ id::NEG_FLOAT;
  float eid_shape_full5x5_sigmaIphiIphi_ = -666; //@@ id::NEG_FLOAT;
  float eid_shape_full5x5_HoverE_ = -666; //@@ id::NEG_FLOAT;
  float eid_shape_full5x5_r9_ = -666; //@@ id::NEG_FLOAT;
  float eid_shape_full5x5_circularity_ = -666; //@@ id::NEG_FLOAT;

  float eid_rho_ = -666; //@@ id::NEG_FLOAT;
  float eid_brem_frac_ = -666; //@@ id::NEG_FLOAT;
  float eid_ele_pt_ = -666; //@@ id::NEG_FLOAT;
  float eid_gsf_bdtout1_ = -666; //@@ id::NEG_FLOAT;

  // Electrons: MVA variables for 2020Sept15
  float eid2_trk_p_ = -666; //@@ id::NEG_FLOAT;
  float eid2_trk_nhits_ = -666; //@@ id::NEG_FLOAT;
  float eid2_trk_chi2red_ = -666; //@@ id::NEG_FLOAT;
  float eid2_trk_dr_ = -666; //@@ id::NEG_FLOAT;

  float eid2_gsf_nhits_ = -666; //@@ id::NEG_FLOAT;
  float eid2_gsf_chi2red_ = -666; //@@ id::NEG_FLOAT;
  float eid2_gsf_mode_p_ = -666; //@@ id::NEG_FLOAT;
  float eid2_gsf_dr_ = -666; //@@ id::NEG_FLOAT;

  float eid2_sc_E_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_eta_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_etaWidth_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_phiWidth_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_Nclus_ = -666; //@@ id::NEG_FLOAT;

  float eid2_match_seed_dEta_ = -666; //@@ id::NEG_FLOAT;
  float eid2_match_eclu_EoverP_ = -666; //@@ id::NEG_FLOAT;
  float eid2_match_SC_EoverP_ = -666; //@@ id::NEG_FLOAT;
  float eid2_match_SC_dEta_ = -666; //@@ id::NEG_FLOAT;
  float eid2_match_SC_dPhi_ = -666; //@@ id::NEG_FLOAT;

  float eid2_shape_full5x5_r9_ = -666; //@@ id::NEG_FLOAT;
  float eid2_shape_full5x5_HoverE_ = -666; //@@ id::NEG_FLOAT;

  float eid2_sc_clus1_nxtal_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus1_E_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus1_E_ov_p_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus1_deta_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus1_dphi_ = -666; //@@ id::NEG_FLOAT;

  float eid2_sc_clus2_E_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus2_E_ov_p_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus2_dphi_ = -666; //@@ id::NEG_FLOAT;
  float eid2_sc_clus2_deta_ = -666; //@@ id::NEG_FLOAT;

  float eid2_rho_ = -666; //@@ id::NEG_FLOAT;
  float eid2_brem_frac_ = -666; //@@ id::NEG_FLOAT;
  float eid2_core_shFracHits_ = -666; //@@ id::NEG_FLOAT;
  float eid2_gsf_bdtout1_ = -666; //@@ id::NEG_FLOAT;

  // Image of electrons for CNN (Max Hart dev's)

  float image_gsf_ref_eta_ = id::NEG_FLOAT;
  float image_gsf_ref_phi_ = id::NEG_FLOAT;
  float image_gsf_ref_R_ = id::NEG_FLOAT;
  float image_gsf_ref_p_ = id::NEG_FLOAT;
  float image_gsf_ref_pt_ = id::NEG_FLOAT;

  float image_gen_inner_eta_ = id::NEG_FLOAT;
  float image_gen_inner_phi_ = id::NEG_FLOAT;
  float image_gen_inner_R_ = id::NEG_FLOAT;
  float image_gen_inner_p_ = id::NEG_FLOAT;
  float image_gen_inner_pt_ = id::NEG_FLOAT;

  float image_gsf_inner_eta_ = id::NEG_FLOAT;
  float image_gsf_inner_phi_ = id::NEG_FLOAT;
  float image_gsf_inner_R_ = id::NEG_FLOAT;
  float image_gsf_inner_p_ = id::NEG_FLOAT;
  float image_gsf_inner_pt_ = id::NEG_FLOAT;
  int image_gsf_charge_ = id::NEG_INT*10;

  float image_gsf_proj_eta_ = id::NEG_FLOAT;
  float image_gsf_proj_phi_ = id::NEG_FLOAT;
  float image_gsf_proj_R_ = id::NEG_FLOAT;
  float image_gsf_proj_p_ = id::NEG_FLOAT;

  float image_gsf_atcalo_eta_ = id::NEG_FLOAT;
  float image_gsf_atcalo_phi_ = id::NEG_FLOAT;
  float image_gsf_atcalo_R_ = id::NEG_FLOAT;
  float image_gsf_atcalo_p_ = id::NEG_FLOAT;

  unsigned int image_clu_n_ = 0;
  //std::vector<float> image_clu_eta_ = {};
  //std::vector<float> image_clu_phi_ = {};
  //std::vector<float> image_clu_e_ = {};
  float image_clu_eta_[ARRAY_SIZE] = {};
  float image_clu_phi_[ARRAY_SIZE] = {};
  float image_clu_e_[ARRAY_SIZE] = {};
  int image_clu_nhit_[ARRAY_SIZE] = {};

  unsigned int image_pf_n_ = 0;
  //std::vector<float> image_pf_eta_ = {};
  //std::vector<float> image_pf_phi_ = {};
  //std::vector<float> image_pf_p_ = {};
  //std::vector<int> image_pf_pdgid_ = {};
  float image_pf_eta_[ARRAY_SIZE] = {};
  float image_pf_phi_[ARRAY_SIZE] = {};
  float image_pf_p_[ARRAY_SIZE] = {};
  int image_pf_pdgid_[ARRAY_SIZE] = {};
  int image_pf_matched_[ARRAY_SIZE] = {};
  int image_pf_lost_[ARRAY_SIZE] = {};

};

#endif // LowPtElectrons_LowPtElectrons_IDNtuple
