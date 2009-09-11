// $Id: Relation.cpp,v 1.7 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Relation
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.10.2004
//COPYRIGHT   : Copyright (C) 2004 - 2006, 2008, 2009

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <boost/shared_ptr.hpp>

#include <YGP/Relation.h>

#include "Test.h"

#ifdef VERBOSE
#  define COUT(x) std::cout << x << '\n';
#else
#  define COUT(x)
#endif


typedef struct Server {
   const char* name;

   Server () { COUT ("Creating server"); }
   ~Server () { COUT ("Deleting server " << name); }
} Server;
typedef boost::shared_ptr<Server> HServer;

typedef struct Client {
   const char* name;

   Client () { COUT ("Creating client"); }
   ~Client () { COUT ("Deleting client " << name); }
} Client;
typedef boost::shared_ptr<Client> HClient;


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   YGP::Relation1_X<HServer, HClient> ServerClient ("ServerClient-1:4", 4);
   HServer hServer (new Server);
   HClient hClient1 (new Client), hClient2 (new Client), hClient3 (new Client);
   HClient hClient4 (new Client), hClient5 (new Client);

   hServer->name = "Server";

   hClient1->name = "Client1";
   ServerClient.relate (hServer, hClient1);
   check (ServerClient.isRelated (hServer, hClient1));

   hClient2->name = "Client2";
   ServerClient.relate (hServer, hClient2);
   check (ServerClient.isRelated (hServer, hClient2));

   hClient3->name = "Client3";
   ServerClient.relate (hServer, hClient3);
   check (ServerClient.isRelated (hServer, hClient3));

   hClient4->name = "Client4";
   ServerClient.relate (hServer, hClient4);
   check (ServerClient.isRelated (hServer, hClient4));

   hClient5->name = "Client5";
   try {
      ServerClient.relate (hServer, hClient5);
      check (!"Overflow caught");
   }
   catch (std::overflow_error& e) {
      check ("Overflow caught");
   }

   check (ServerClient.getParent (hClient1) == hServer);
   check (ServerClient.getParent (hClient2) == hServer);
   check (ServerClient.getParent (hClient3) == hServer);
   check (ServerClient.getParent (hClient4) == hServer);

   check (*(ServerClient.getObjects (hServer).begin ()) == hClient1);
   check (*(ServerClient.getObjects (hServer).begin ()) != hClient2);
   check (*(ServerClient.getObjects (hServer).rbegin ()) == hClient4);
   check (ServerClient.getObjects (hServer).size () == 4);

   check (ServerClient.isRelated (hServer));
   check (ServerClient.isRelated (hClient1));
   ServerClient.unrelate (hServer, hClient1);
   check (!ServerClient.isRelated (hClient1));

   check (ServerClient.isRelated (hServer));
   check (ServerClient.isRelated (hClient2));
   ServerClient.unrelate (hServer, hClient2);
   check (!ServerClient.isRelated (hClient2));

   check (ServerClient.isRelated (hServer));
   check (ServerClient.isRelated (hClient3));
   ServerClient.unrelate (hServer, hClient3);
   check (!ServerClient.isRelated (hClient3));

   check (ServerClient.isRelated (hServer));
   check (ServerClient.isRelated (hClient4));
   ServerClient.unrelate (hServer, hClient4);
   check (!ServerClient.isRelated (hClient4));
   check (!ServerClient.isRelated (hServer));

   std::vector<HClient> vClients;
   vClients.push_back (hClient2);
   vClients.push_back (hClient3);
   vClients.push_back (hClient4);
   ServerClient.relate (hServer, vClients);
   check (ServerClient.isRelated (hServer));
   check (!ServerClient.isRelated (hClient1));
   check (ServerClient.isRelated (hClient4));
   check (ServerClient.getObjects (hServer).size () == vClients.size ());

   YGP::Relation1_1<HServer, HClient> sc1 ("ServerClient-1:1");
   check (!sc1.isRelated (hServer, hClient1));
   sc1.relate (hServer, hClient1);
   check (sc1.isRelated (hServer, hClient1));

   try {
      sc1.relate (hServer, hClient2);
      check (!"Overflow caught");
   }
   catch (std::overflow_error& e) {
      check ("Overflow caught");
   }
   sc1.unrelate (hServer, hClient1);
   check (!sc1.isRelated (hServer, hClient1));

   YGP::RelationN_M<HServer, HClient> scx ("ServerClient-N:M");

   HServer hServer2 (new Server);
   hServer2->name = "Server2";

   check (!scx.isRelated (hServer, hClient1));
   scx.relate (hServer, hClient1);
   check (scx.isRelated (hServer, hClient1));
   scx.relate (hServer, hClient2);
   check (scx.isRelated (hServer, hClient2));
   scx.relate (hServer, hClient3);
   check (scx.isRelated (hServer, hClient3));
   scx.relate (hServer, hClient4);
   check (scx.isRelated (hServer, hClient4));

   scx.relate (hServer2, hClient5);
   check (scx.isRelated (hServer2, hClient5));
   scx.relate (hServer2, hClient1);
   check (scx.isRelated (hServer2, hClient1));
   scx.relate (hServer2, hClient2);
   check (scx.isRelated (hServer2, hClient2));

   check (scx.getParents (hClient1).size () == 2);
   check (scx.getParents (hClient2).size () == 2);
   check (scx.getParents (hClient3).size () == 1);
   check (scx.getParents (hClient4).size () == 1);
   check (scx.getParents (hClient5).size () == 1);

   check (scx.getObjects (hServer).size () == 4);
   check (scx.getObjects (hServer2).size () == 3);

   scx.unrelate (hServer, hClient1);
   check (scx.getObjects (hServer).size () == 3);

   scx.unrelateAll (hServer);
   check (!scx.isRelated (hServer));
   check (scx.getParents (hClient1).size () == 1);

   scx.relate (hServer, vClients);
   check (scx.getObjects (hServer).size () == vClients.size ());

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
