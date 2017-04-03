#ifndef EVENTPROXYTEST_h
#define EVENTPROXYTEST_h

#include <string>
#include <typeinfo>
#include <vector>
#include <iostream>
#include "boost/shared_ptr.hpp"

#include "EventProxyBase.h"

#include "TBranch.h"
#include "TLorentzVector.h"

#include "TriggerTypeDefs.h"
//////////////////////////////////////////////
//////////////////////////////////////////////
class EventProxyTest : public EventProxyBase {

public:

EventProxyTest();
virtual ~EventProxyTest();

void init(std::vector<std::string> const& iFileNames);

virtual EventProxyBase* clone() const;

std::vector<float> floats_jetNtupleMaker_jetEta_jetAnalysis_obj;
std::vector<float> floats_jetNtupleMaker_jetMass_jetAnalysis_obj;
std::vector<float> floats_jetNtupleMaker_jetPhi_jetAnalysis_obj;
std::vector<float> floats_jetNtupleMaker_jetPt_jetAnalysis_obj;

std::vector<float>   floats_muonNtupleMaker_muonEta_PAT_obj;
std::vector<float>   floats_muonNtupleMaker_muonPhi_PAT_obj;
std::vector<float>   floats_muonNtupleMaker_muonPt_PAT_obj;

static const Int_t kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj = 699;
Bool_t patTriggerObjectStandAlones_patTrigger__PAT_present;
Int_t patTriggerObjectStandAlones_patTrigger__PAT_obj_;
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fX[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fY[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_vertex__fCoordinates_fZ[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fPt[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fEta[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fPhi[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Double32_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_p4Polar__fCoordinates_fM[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];           //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Int_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_qx3_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];                //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Int_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_pdgId_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];                //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
Int_t patTriggerObjectStandAlones_patTrigger__PAT_obj_m_state_status_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];                //[patTriggerObjectStandAlones_patTrigger__PAT.obj_]
std::string patTriggerObjectStandAlones_patTrigger__PAT_obj_collection_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<trigger::TriggerObjectType> patTriggerObjectStandAlones_patTrigger__PAT_obj_triggerObjectTypes_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<std::string>  patTriggerObjectStandAlones_patTrigger__PAT_obj_filterLabels_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<std::string>  patTriggerObjectStandAlones_patTrigger__PAT_obj_pathNames_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<unsigned short> patTriggerObjectStandAlones_patTrigger__PAT_obj_pathIndices_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<bool>    patTriggerObjectStandAlones_patTrigger__PAT_obj_pathLastFilterAccepted_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];
std::vector<bool>    patTriggerObjectStandAlones_patTrigger__PAT_obj_pathL3FilterAccepted_[kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj];

unsigned int getNumberOfJets() const {
        return floats_jetNtupleMaker_jetEta_jetAnalysis_obj.size();
};
TLorentzVector getJetP4(unsigned int iJet) const;

};
#endif
