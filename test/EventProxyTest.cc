#include "EventProxyTest.h"

#include "TLeaf.h"

#include <iostream>

EventProxyTest::EventProxyTest(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyTest::~EventProxyTest(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyBase* EventProxyTest::clone() const{

  return new EventProxyTest();

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void EventProxyTest::init(std::vector<std::string> const& iFileNames){

	treeName_ = "Events";

	EventProxyBase::init(iFileNames);

	fChain->SetMakeClass(1);
  fChain->SetBranchStatus("*",0);
	fChain->SetBranchStatus("floats_*",1);
  fChain->SetBranchStatus("patTriggerObjectStandAlones_patTrigger__PAT.obj*",1);
  fChain->SetBranchStatus("patTriggerObjectStandAlones_patTrigger__PAT.present",1);
  fChain->SetBranchStatus("patTriggerObjectStandAlones_patTrigger__PAT.obj.triggerObjectTypes_",1);
  fChain->SetBranchStatus("patTriggerObjectStandAlones_patTrigger__PAT.obj.filterLabels_*",1);
  //fChain->SetBranchStatus("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.p4Polar_.fCoordinates.fPt",1);

	fChain->SetBranchAddress("floats_jetNtupleMaker_jetEta_PAT.obj", &floats_jetNtupleMaker_jetEta_jetAnalysis_obj);
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetMass_PAT.obj", &floats_jetNtupleMaker_jetMass_jetAnalysis_obj);
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetPhi_PAT.obj", &floats_jetNtupleMaker_jetPhi_jetAnalysis_obj);
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetPt_PAT.obj", &floats_jetNtupleMaker_jetPt_jetAnalysis_obj);

   fChain->SetBranchAddress("floats_muonNtupleMaker_muonEta_PAT.obj", &floats_muonNtupleMaker_muonEta_PAT_obj);
   fChain->SetBranchAddress("floats_muonNtupleMaker_muonPhi_PAT.obj", &floats_muonNtupleMaker_muonPhi_PAT_obj);
   fChain->SetBranchAddress("floats_muonNtupleMaker_muonPt_PAT.obj", &floats_muonNtupleMaker_muonPt_PAT_obj);


   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj", &patTriggerObjectStandAlones_patTrigger__PAT_obj_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.present",&patTriggerObjectStandAlones_patTrigger__PAT_present);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.vertex_.fCoordinates.fX", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fX);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.vertex_.fCoordinates.fY", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fY);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.vertex_.fCoordinates.fZ", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fZ);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.p4Polar_.fCoordinates.fPt", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fPt);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.p4Polar_.fCoordinates.fEta", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fEta);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.p4Polar_.fCoordinates.fPhi", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fPhi);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.p4Polar_.fCoordinates.fM", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fM);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.qx3_", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_qx3_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.pdgId_", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_pdgId_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.m_state.status_", patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_status_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.collection_", patTriggerObjectStandAlones_patTrigger__PAT_obj_collection_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.triggerObjectTypes_", patTriggerObjectStandAlones_patTrigger__PAT_obj_triggerObjectTypes_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.filterLabels_", patTriggerObjectStandAlones_patTrigger__PAT_obj_filterLabels_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.pathNames_", patTriggerObjectStandAlones_patTrigger__PAT_obj_pathNames_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.pathIndices_", patTriggerObjectStandAlones_patTrigger__PAT_obj_pathIndices_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.pathLastFilterAccepted_", patTriggerObjectStandAlones_patTrigger__PAT_obj_pathLastFilterAccepted_);
   fChain->SetBranchAddress("patTriggerObjectStandAlones_patTrigger__PAT.obj.pathL3FilterAccepted_", patTriggerObjectStandAlones_patTrigger__PAT_obj_pathL3FilterAccepted_);

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
TLorentzVector EventProxyTest::getJetP4(unsigned int iJet) const {

  TLorentzVector aP4;

  if(iJet>=floats_jetNtupleMaker_jetPt_jetAnalysis_obj.size()) return aP4;

  aP4.SetPtEtaPhiM(floats_jetNtupleMaker_jetPt_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetEta_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetPhi_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetMass_jetAnalysis_obj[iJet]);

    return aP4;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
