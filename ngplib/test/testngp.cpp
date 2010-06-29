#include "NGP.h"

#include "Common.h"

#include "NavNopReq.h"
#include "NavSearchDescReq.h"
#include "NavSearchDescReply.h"

#include <iostream>
#include <assert.h>

int main() {
   
   // Test NavSearchDescReq
   NavSearchDescReq r1(LanguageCodeEnum::SWEDISH);
   NParamBlock p;
   r1.serialize(p);

   NavSearchDescReq r2(p);

   assert(r1.getClientLanguage() == r2.getClientLanguage() &&
          r2.getClientLanguage() == LanguageCodeEnum::SWEDISH);

   // Test NavSearchDescReply
   std::vector< NavSearchDescReply::SearchHitType > searchHits;
   searchHits.push_back(NavSearchDescReply::SearchHitType(0,1,"test1", 2, "test2"));
   searchHits.push_back(NavSearchDescReply::SearchHitType(1,2,"test2", 3, "test3"));
   
   NavSearchDescReply r3(searchHits, "testcrc");
   NParamBlock p3;
   r3.serialize(p3);

   NavSearchDescReply r4(p3);

   assert(r3.getSearchHitType().size() == r4.getSearchHitType().size() &&
          r4.getSearchHitType().size() == 2);

   assert(r3.getSearchDescCrc() == r4.getSearchDescCrc() &&
          r4.getSearchDescCrc() == "testcrc");

   assert(r3.getSearchHitType()[0].round == r4.getSearchHitType()[0].round &&
          r4.getSearchHitType()[0].round == 0);

   assert(r3.getSearchHitType()[0].heading == r4.getSearchHitType()[0].heading &&
          r4.getSearchHitType()[0].heading == 1);

   assert(r3.getSearchHitType()[0].name == r4.getSearchHitType()[0].name &&
          r4.getSearchHitType()[0].name == "test1");
   
   assert(r3.getSearchHitType()[0].topRegionId == r4.getSearchHitType()[0].topRegionId &&
          r4.getSearchHitType()[0].topRegionId == 2);
   
   assert(r3.getSearchHitType()[0].imageName == r4.getSearchHitType()[0].imageName &&
          r4.getSearchHitType()[0].imageName == "test2");


   assert(r3.getSearchHitType()[1].round == r4.getSearchHitType()[1].round &&
          r4.getSearchHitType()[1].round == 1);

   assert(r3.getSearchHitType()[1].heading == r4.getSearchHitType()[1].heading &&
          r4.getSearchHitType()[1].heading == 2);

   assert(r3.getSearchHitType()[1].name == r4.getSearchHitType()[1].name &&
          r4.getSearchHitType()[1].name == "test2");
   
   assert(r3.getSearchHitType()[1].topRegionId == r4.getSearchHitType()[1].topRegionId &&
          r4.getSearchHitType()[1].topRegionId == 3);
   
   assert(r3.getSearchHitType()[1].imageName == r4.getSearchHitType()[1].imageName &&
          r4.getSearchHitType()[1].imageName == "test3");

   assert(r3 == r4);

   std::cout << "Everything OK so far." << std::endl;
}
