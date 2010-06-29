/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

/**
 * The SecurityManager handles authorization of the user and making sure the
 * user only can do what it has the rights for.
 *
 * Possible solutions:
 * A. The SecurityManager downloads the users rights from server at
 * startup. When someone later askes for the rights it can get an answer
 * directly.
 * B. The SecurityManager checks the users rights at runtime, meaning it takes
 * some time for the SecurityManager to answer the question.
 * C. Non of above is needed! =)
 */
class SecurityManager {

public:
   
   /**
    * Constructor.
    */
   SecurityManager();

   /**
    * Destructor.
    */
   ~SecurityManager();

   /**
    * Check what rights the user has paid for.
    */
   void checkUserRights();

   /**
    * Check if the user has paid for a specific right.
    *
    * @param right The specific right to check.
    */
   void checkSpecificRight(/*enum Rights right*/);

   /**
    * Check if the user is authorized for the wayfinder services.
    */
   void isUserAuthorized();

}

#endif
