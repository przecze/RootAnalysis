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
//////////////////////////////////////////////
//////////////////////////////////////////////
   class EventProxyTest: public EventProxyBase{

   public:

      EventProxyTest();
      virtual ~EventProxyTest();

      void init(std::vector<std::string> const& iFileNames);

      virtual EventProxyBase* clone() const;

       std::vector<float> floats_jetNtupleMaker_jetEta_jetAnalysis_obj;
       std::vector<float> floats_jetNtupleMaker_jetMass_jetAnalysis_obj;
       std::vector<float> floats_jetNtupleMaker_jetPhi_jetAnalysis_obj;
       std::vector<float> floats_jetNtupleMaker_jetPt_jetAnalysis_obj;
       
   unsigned int getNumberOfJets() const {return floats_jetNtupleMaker_jetEta_jetAnalysis_obj.size();};
   TLorentzVector getJetP4(unsigned int iJet) const;
      
   };
#endif
