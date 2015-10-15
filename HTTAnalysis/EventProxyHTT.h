#ifndef EVENTPROXYHTT_h
#define EVENTPROXYHTT_h

#include <string>
#include <typeinfo>
#include <vector>
#include "boost/shared_ptr.hpp"

#include "EventProxyBase.h"
#include "HTTEvent.h"

#include "TBranch.h"

   class EventProxyHTT: public EventProxyBase{

   public:

      EventProxyHTT();
      virtual ~EventProxyHTT();

      void init(std::vector<std::string> const& iFileNames);

      int npv;
      int run;

      float svfit;
      float puWeight;
      float genWeight;

      Wevent *wevent;
      std::vector<Wpair>  *wpair;
      
   private:

      // List of branches
      TBranch        *b_npv;
      TBranch        *b_run;

      TBranch        *b_svfit;
      TBranch        *b_puWeight;
      TBranch        *b_genWeight;

   };
#endif
