//$Id: XMessageBox.cpp,v 1.10 2002/07/08 03:38:47 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XMessageBox
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.9.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include <gtk--/box.h>
#include <gtk--/main.h>
#include <gtk--/label.h>
#include <gtk--/button.h>
#include <gtk--/pixmap.h>

#include "Trace_.h"

#include "Internal.h"
#include "XMessageBox.h"


const char* const XMessageBox::titles[] = { N_("Info"), N_("Question"), N_("Warning"),
                                            N_("Error"), N_("Critical") };
const char* const XMessageBox::labels[] = { N_("OK"), N_("Retry"), N_("Yes"), N_("No"),
                                            N_("Cancel") };
const char* const * const XMessageBox::icons[] = { iconInfo, iconQuestion, iconWarning,
                                                   iconError, iconCritical };

const char* const XMessageBox::iconInfo[] = {
   /* columns rows colors chars-per-pixel */
   "32 32 4 1",
   "  c #000000",
   ". c #0000be",
   "X c #ffffff",
   "o c None",
   /* pixels */
   "oooooooooooooooooooooooooooooooo",
   "oooooooooooo........oooooooooooo",
   "ooooooooo..............ooooooooo",
   "oooooooo................oooooooo",
   "oooooo.........XXXX.......oooooo",
   "ooooo..........XXXX........ooooo",
   "oooo...........XXXX.........oooo",
   "oooo...........XXXX.........oooo",
   "ooo..........................ooo",
   "oo............................oo",
   "oo.........XXXXXXXX...........oo",
   "oo.........XXXXXXXX...........oo",
   "o..........XXXXXXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "o..............XXXX............o",
   "oo.............XXXX...........oo",
   "oo.............XXXX...........oo",
   "oo.............XXXX...........oo",
   "ooo............XXXX..........ooo",
   "oooo...........XXXX.........oooo",
   "oooo.......XXXXXXXXXXX......oooo",
   "ooooo......XXXXXXXXXXX.....ooooo",
   "oooooo.....XXXXXXXXXXX....oooooo",
   "oooooooo................oooooooo",
   "ooooooooo..............ooooooooo",
   "oooooooooooo........oooooooooooo",
   "oooooooooooooooooooooooooooooooo" };

const char* const XMessageBox::iconCritical[] = {
   "38 42 40 1",
   "       c None",
   ".      c #514451445144",
   "X      c #208120812081",
   "o      c #208110402081",
   "O      c #208120811040",
   "+      c #861771C68617",
   "@      c #EFBE51441040",
   "#      c #EFBE10402081",
   "$      c #000000000000",
   "%      c #EFBE00002081",
   "&      c #DF7DDF7D1040",
   "*      c #DF7DDF7D0000",
   "=      c #EFBE00001040",
   "-      c #EFBE10401040",
   ";      c #30C2208130C2",
   ":      c #208130C22081",
   ">      c #EFBE51442081",
   ",      c #71C671C66185",
   "<      c #9E79A2899E79",
   "1      c #EFBEDF7D0000",
   "2      c #EFBE41032081",
   "3      c #104000001040",
   "4      c #104010400000",
   "5      c #30C230C22081",
   "6      c #71C6618571C6",
   "7      c #000010400000",
   "8      c #104000000000",
   "9      c #71C6820771C6",
   "0      c #618551445144",
   "q      c #410330C230C2",
   "w      c #71C671C671C6",
   "e      c #514441035144",
   "r      c #4103410330C2",
   "t      c #618571C66185",
   "y      c #410341034103",
   "u      c #514461855144",
   "i      c #618551446185",
   "p      c #30C2410330C2",
   "a      c #861771C671C6",
   "s      c #8617820771C6",
   "                     ..XXXX.            ",
   "                    .oOXOoOXO.          ",
   "                   .XX.   +.XX. @ #     ",
   "                  .XO.      .X$.%%@     ",
   "                  .$$.       %&@#@      ",
   "                 $.X$$X    %%%@*&=-%    ",
   "                ;XXX$$$:    %@&&&@>     ",
   "               ,$.$$$$$$   %<%@1&2-     ",
   "              ,$.XX.$:$$,   # #@@-2-    ",
   "            ,$$33333X3X3$,    %2==      ",
   "           $$$$33$$$$$$$$$$,  %  =      ",
   "         ,$$333$$3$$$$$3$33$,           ",
   "        $$$33$$$$$$$$$$33$3$$$          ",
   "       $$$$$$$$$$$$$$$$$$3<..$$         ",
   "      $33344<<..$$$$$$$$$33@@.$$        ",
   "     33334<<..5$$$$$$$$$$33.<@$$$       ",
   "    33$34<..X4$$$$$$$$$$$3333.$$$$      ",
   "   $3$34<..4$$$33$$$$$$$$$$33$$$$$$     ",
   "  .3333<.X4$$333$$$$$$$$$$$$$33$$$$.    ",
   " 6$3334<.4$$3$$$$$$$$$$$$$$$$$33$$$$    ",
   " X$33$<.;$3$$$$$$$$$$$$$$$$$$$$$$$$$:   ",
   ",$$3$4<.433$$$$$$$$$$$$$$$$$$$$$$$$$$   ",
   ".$3$$..:43$$$$$$$78$$$$$$$$$$$$$$$$$$,  ",
   "X$33$.5;$3$$$$$4$878$$$$$$$$$$$$$$$$$.  ",
   "$$33$.;$33$$$$4887$787$$$$$$$$$$$$$$$X  ",
   "$$33$;$333$$$$$878$878$8$$$$$$$$$$$$$$  ",
   "$333$$$$3$$$$$8787878787$$$$$$$$$3$$$$  ",
   "$$333$$333$$$848$878$878$$$$$8$$$3$$$$  ",
   "$$33$$33$3$$$8$787$787$$$$$$$$$$$3$$$$  ",
   "X$33$4333$$$$$$8788878$$$$$$$$$$3$$3$$  ",
   ".$33$X7333$$$$$$$787$8$$$$$$$$$3$433$X  ",
   ",$$3$4$3333$$$$$$$$$$$$$$$$$$$33$73$$.  ",
   " $$3$X43$$$$$$$$$$$$$$$$$$$$$$3$4$$$$,  ",
   " .$$$X433$$$$$$$$$$$$$$$$$$$33$433$$$   ",
   "  $$$44X333$$$$$$$$$$$$$$$$$3$433$$$.   ",
   "  ,$$$X4333$$$$$$$$$$$$$$$334$$$$$$X    ",
   "   $$$$X4833$$$$$$$$$$$$3343333$3$$9    ",
   "    $$$$4X333$$$$$$$$$$333343$3$3$0     ",
   "     $$$$4X473$$$$$$$$3$334333$$$Xq0w   ",
   "      $$$$44X43$333333$$433$33$$XXXq0e  ",
   "       $$$$$4X4X4$33$3433$$33$$XXXXXe0e ",
   "        $$$$$$$$$$$33$3$$3$$$$XXXXXXe000",
   "        w6$$$3333333$3333$$$XXXXXXXqee00",
   "          w6$$$$$3$33$$$$$XXXXXXXXqe000w",
   "            66XX$$$$$$XXXXrXrXrXqee00ww ",
   "               t6t......yuipiuiw0awws   " };

const char* const XMessageBox::iconQuestion[] = {
   "42 33 64 1",
   "       c None",
   ".      c #415151",
   "+      c #D7DBDF",
   "@      c #BEB2C7",
   "#      c #BECBBE",
   "$      c #9EA6A6",
   "%      c #86868E",
   "&      c #717979",
   "*      c #697171",
   "=      c #696961",
   "-      c #616171",
   ";      c #616161",
   ">      c #516151",
   ",      c #494961",
   "'      c #515151",
   ")      c #616151",
   "!      c #201020",
   "~      c #202010",
   "{      c #202020",
   "]      c #201828",
   "^      c #302030",
   "/      c #283028",
   "(      c #303030",
   "_      c #102010",
   ":      c #101010",
   "<      c #516161",
   "[      c #101020",
   "}      c #102020",
   "|      c #203020",
   "1      c #384130",
   "2      c #413030",
   "3      c #201010",
   "4      c #000000",
   "5      c #717196",
   "6      c #696986",
   "7      c #86869E",
   "8      c #494151",
   "9      c #304141",
   "0      c #413451",
   "a      c #415141",
   "b      c #101000",
   "c      c #080810",
   "d      c #080800",
   "e      c #515171",
   "f      c #615171",
   "g      c #615186",
   "h      c #516171",
   "i      c #516186",
   "j      c #61558E",
   "k      c #614171",
   "l      c #715186",
   "m      c #413041",
   "n      c #616196",
   "o      c #282841",
   "p      c #716196",
   "q      c #617196",
   "r      c #796196",
   "s      c #71719E",
   "t      c #86719E",
   "u      c #8675A6",
   "v      c #615151",
   "w      c #615161",
   "x      c #514141",
   "y      c #515141",
   " .++++++++@#@$@$$$$$$$%%%%%&&&*=-;;>,.'*  ",
   ".+)!~{{]{^/^(^/^((((((/^(^/^/^{]{{~!_!::: ",
   "+<[_{}]|^|(/(/(((((((((((/(/(|]|{}{}!_[::1",
   "+!~{{]|^/((((2(2121212(2(2((/^/^|]{{~!_3:4",
   "+{{{567%&*-<890909090909090a,<-*%77$5%{_[4",
   "+{|^{{~!_!:3::bcdc4d4ddcbcbc:::!_!~!{{~!:4",
   "+|(({eeefeghghgigigjjigjghgighgefeeee}]{{4",
   "+^(2~kefegfg-gjgjljgjljgjgjg-gfgefef,!{{~4",
   "@(m1!egegjgjjjnjjj,/]}oejjnjgjgighgee}^{{4",
   "#m1m:gegjgjljjjjj!}ljo44]jjljgjgfgefe!|^{4",
   "+10a:hgjjjnjpjpq04[qpj4448pjpjnjjhghg_(/^4",
   "#m08:g-gjljpjppp{4405p{44^ppjpjgjg-ge3((|4",
   "@a8.:jjjnjpnr5r5044,r5]44{pqpnpjjjgig:(/^4",
   "#8a'bgjljpjppr5r5,,rsr:4405pppjjjgjgfc(^/4",
   "+''>:ijjpnpqr5sstsuuth44!qrqpqpjnjgjgdo((4",
   "#v'wdgjpjppp5rsttttut]4]5r5p5p6pjljg-c((/4",
   "@>,<4jnjpqp5rstsu7u7gc,stss5rqpjnjjjgdm((4",
   "#''wdgjjjp5p5rsttu7u]gstsr5pppjpjjjgfc12(4",
   "+>,<:jnjpqr5s5suuuu7]uust5r5pqpjnjgjgcm1(4",
   "#'>;bgjljppp5r5tstttjtst5r5p5p6pjljg-:12/4",
   "@'w;:jjjnnpqr5s5sststst5r5rqpqpjnjgig_m((4",
   "#'>w:gjgjpjppp5p5r50d!jp5pppjpjljg-gf!((/4",
   "+a,<[hgjjjnnpqpqpqn444oqpqpqpjnjjighg_o((4",
   "#8'w_ffg-ljljpjpjpe444{pjpjpjljg-gfge!((|4",
   "@a'>!eghgjgjjjnjnjpo4:ejnjnjjjgighgee}(/^4",
   "#0av~keffg-gjgjljgjljjjljgjgjg-gfgeke{/^{4",
   "+98.{eeefeghgigjjjgjjjgjjigighghfeeee}(|{4",
   "#21x~!{!_!::::bcdd444d4cdc:::3:!_!{{~!|]~4",
   "@(m9567%&*-<890909090909090a,<-*%77$5%]}!4",
   "#^/210a8'w>w;;;=;;)w)w>w''y8a80m1m(^|{~!:4",
   "+}]/(10a8a'>'>,>w>,>'>,''a8a09m1(/(|{{{_:4",
   "#!|^(210a8'''v>w>v'v'v''y8ax0m1m(^/^{{~!:4",
   "@44444444444444444444444444444444444444444" };

const char* const XMessageBox::iconWarning[]={
   "27 19 2 1",
   "# c #FF0000",
   ". c None",
   "#########...............#..",
   ".#########.............#...",
   "..#########...........#....",
   "...#########.........#.....",
   "....#########.......#......",
   ".....#########.....#.......",
   "......#########...#........",
   ".......#######...#.........",
   "........#####...#..........",
   ".........###...###.........",
   "..........#...#####........",
   ".........#...#######.......",
   "........#...#########......",
   ".......#.....#########.....",
   "......#.......#########....",
   ".....#.........#########...",
   "....#...........#########..",
   "...#.............#########.",
   "..#...............#########" };

const char* const XMessageBox::iconError[] = {
   "45 47 63 1",
   "       c None",
   ".      c #CF3CCB2BCF3C",
   "X      c #EFBEEBADEFBE",
   "o      c #D75CD34CD75C",
   "O      c #514451445144",
   "+      c #9E799A699E79",
   "@      c #FFFFFBEEFFFF",
   "#      c #E79DE38DE79D",
   "$      c #082008200820",
   "%      c #965892489658",
   "&      c #A699A289A699",
   "*      c #BEFBBAEABEFB",
   "=      c #410341034103",
   "-      c #861782078617",
   ";      c #8E388A288E38",
   ":      c #DF7DDB6CDF7D",
   ">      c #F7DEF3CEF7DE",
   ",      c #208120812081",
   "<      c #000000000000",
   "1      c #28A228A228A2",
   "2      c #B6DAB2CAB6DA",
   "3      c #30C230C230C2",
   "4      c #596559655965",
   "5      c #96589A699E79",
   "6      c #71C671C671C6",
   "7      c #69A669A669A6",
   "8      c #618561856185",
   "9      c #30C228A228A2",
   "0      c #DF7DD34CDF7D",
   "q      c #BEFBB2CABEFB",
   "w      c #492449244924",
   "e      c #104010401040",
   "r      c #104010400820",
   "t      c #8E3892489658",
   "y      c #AEBAB2CAAEBA",
   "u      c #9E7992489E79",
   "i      c #186118611861",
   "p      c #86178A288617",
   "a      c #79E779E779E7",
   "s      c #596559655144",
   "d      c #C71BC30BC71B",
   "f      c #F7DEFBEEFFFF",
   "g      c #AEBAA289A699",
   "h      c #208128A22081",
   "j      c #E79DEBADE79D",
   "k      c #28A230C230C2",
   "l      c #38E338E338E3",
   "z      c #FFFFF3CEF7DE",
   "x      c #9E7992489658",
   "c      c #79E771C671C6",
   "v      c #EFBEE38DE79D",
   "b      c #BEFBC30BC71B",
   "n      c #A699A2899E79",
   "m      c #C71BCB2BCF3C",
   "M      c #492451445144",
   "N      c #861779E78617",
   "B      c #CF3CD34CCF3C",
   "V      c #AEBAAAAAB6DA",
   "C      c #A699A289AEBA",
   "Z      c #D75CCB2BCF3C",
   "A      c #F7DEF3CEEFBE",
   "S      c #79E782078617",
   "D      c #596551445144",
   "              .X                             ",
   "              XoO+                           ",
   "              @#O$%              &X %        ",
   "             *@#%=$-            o@o$;        ",
   "             :>:o%,<O          X@X%1         ",
   "             XX: 2-3$,O4- +  5:@>>&=         ",
   "             >:;-67-88=9,11 @@@#.0-=         ",
   "             @qO4w1,13er$$,7ty u&.O3         ",
   "            &@4e,,ie<<<<<<<<e4;p% =1         ",
   "            o2=,r<<<<<<<<<<<<<=Oa%ew         ",
   "      +5   o*,<<<<<<<<<<<<<<<<<,=7i8         ",
   "      >+ws-21<<<<<<<<<<<<<<<<<<<e933+     do&",
   "     :@d%;8e<<<<<<<<<<<<<<<<<<<<<eO&#XXX@@@d;",
   "X>:oo@* +8$<<<<<<<<<<<<<<<<<<<<<<<w5o>>f>Xg= ",
   "o@@@@@2&6e<<<<<<<<<<<<<<<<<<<<<<<<h6dX#0Xjkw ",
   ".@.#:d&-1<<<<<<<<<<<<<<<<<<<<<<<<<$Od>#.:%<% ",
   " @..2&-l$<<<<<<<<<<<<<<<<<<<<<<<<<<l.z#.*l3  ",
   " ##2%6wi<<<<<<<<<<<<<<<<<<<<<<<<<<<hd>X*x$6  ",
   " .@+;63r<<<<<<<<<<<<<<<<<<<<<<<<<<<<6>Xd6i   ",
   "  X*uck$<<<<<<<<<<<<<<<<<<<<<<<<<<<<3v#:lO   ",
   "  b>n6l$<<<<<<<<<<<<<<<<<<<<<<<<<<<<,0:%<    ",
   "   X*al$<<<<<<<<<<<<<<<<<<<<<<<<<<<<<m#13    ",
   "   d>aM$<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*;$%    ",
   "   2@26<<<<<<<<<<<<<<<<<<<<<<<<<<<<<eN$8     ",
   "   X>&4$<<<<<<<<<<<<<<<<<<<<<<<<<<<<elO      ",
   "  :@.;=e<<<<<<<<<<<<<<<<<<<<<<<<<<<<,ww      ",
   " *@X*73,<<<<<<<<<<<<<<<<<<<<<<<<<<<<11$      ",
   " :fo%7=i<<<<<<<<<<<<<<<<<<<<<<<<<<<<=a<7     ",
   " X>*uaw,<<<<<<<<<<<<<<<<<<<<<<<<<<<r :e=     ",
   " @>2awl,,<<<<<<<<<<<<<<<<<<<<<<<<<<8:Ba1     ",
   ".XV3e$ekw$<<<<<<<<<<<<<<<<<<<<<<<<1+>b11     ",
   "#d1eO;  6-r<<<<<<<<<<<<<<<<<<<<<<$7X>&l1     ",
   "X4,tC   ;>2$<<<<<<<<<<<<<<<<<<<<<=#@#&3=     ",
   "d,+      d@7h<<<<<<<<<<<<<<<<<<$wd@zoq41     ",
   "6-       o@&81e$<<<<<<<<<<<<<145:@>#.Zae6    ",
   "         d>#x=1e<<<<<<<<<<<ia2#0A>#:ZZ2,=    ",
   "         *XA*-==,i$<<<<<$l6*X@X0X#o.m..4<+   ",
   "          X>d%784886M8-a% C.@@XX@>voZ:2ae8   ",
   "         &#@dt;-SNux-&&6=3O2X@ --dXvZ %Oi=   ",
   "         &d@0&+%;NDi<<<11$$6o*<<3    &x=$1   ",
   "          q@v* &g8<lO-    8O:r<6             ",
   "           j@o*2;kO        %6<8              ",
   "           +X>Bd8i%          ;               ",
   "            yXz.8i                           ",
   "             gb@nr                           ",
   "              ;a8<%                          ",
   "                &7-                          " };


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; adds all controls to the dialog
//Parameters: text: Text of MessageBox
//            title: Title of MessageBox
//            flags: Flags for display; indicates type of MessageBox and the
//                   Buttons which should be displayed
/*--------------------------------------------------------------------------*/
XMessageBox::XMessageBox (const string& text, const string& title,
                          int flags, unsigned int defButton)
   : Dialog (), txt (new Label (text)), client (new HBox ())
   , ret (CANCEL) {
   assert (txt); assert (client);

   TRACE1 ("XMessageBox::XMessageBox (const string&, const string&,"
           " int, unsigned int)\n - Text: " << text << "\nTitle: " << title);

   set_modal (true);

   if (title.empty ()) {
      assert ((flags & TYPEMASK) < (sizeof (titles) / sizeof (titles[0])));
      TRACE8 ("XMessageBox::XMessageBox (const string&, const string&,"
              " int, unsigned int)\n - Defaulttitle: " << titles[flags & TYPEMASK]);
      set_title (_(titles[flags & TYPEMASK]));
   }
   else
      set_title (title);

   assert ((flags & TYPEMASK) < (sizeof (icons) / sizeof (icons[0])));
   assert (icons[flags & TYPEMASK]);

   icon = new Pixmap (icons[flags & TYPEMASK]); assert (icon);
   client->pack_start (*icon, false, false, 5);

   if (!(flags & ~TYPEMASK))       // Assure, that there's at least one button
      flags |= OK;

   // Check which buttons should be set
   for (unsigned int i (0); i < sizeof (labels) / sizeof (labels[0]); ++i)
      if (flags & (1 << (i + TYPEBITS))) {
         TRACE5 ("XMessageBox::XMessageBox (const string&, const string&,"
                 " int, unsigned int)\n - Add Button " << labels[i]);

         Button* temp (new Button (_(labels[i])));            // Create button

	 temp->clicked.connect (bind (slot (this, &XMessageBox::perform),
                                      1 << (i + TYPEBITS)));

         get_action_area ()->pack_start (*temp, false, false, 5);   // and add
         buttons.push_back (temp);

         temp->set_flags (GTK_CAN_DEFAULT);
         temp->set_usize (90, 35);
         if (i == defButton)
            temp->grab_default ();
      } // endif button to set

   client->pack_start (*txt, true, false, 5);          // Put text into client
   get_vbox ()->pack_start (*client, true, false, 5);

   show_all ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XMessageBox::~XMessageBox () {
   TRACE9 ("XMessageBox::~XMessageBox ()");

   for (vector<Button*>::iterator i (buttons.begin ());
        i != buttons.end (); ++i)
      delete *i;

   TRACE5 ("XMessageBox::~XMessageBox - returning " << ret);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
/*--------------------------------------------------------------------------*/
void XMessageBox::perform (int action) {
   TRACE9 ("XMessageBox::perform (int) - Action = " << action);

   ret = action;
   Gtk::Main::quit();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Displays and performs the message-box
//Parameters: text: Text of the message
//            title: Title of the window; if empty us default-text according
//                   to the passed flags
//            flags: Bitset for buttons to show and type of message
//            defButton: Number of default-button
/*--------------------------------------------------------------------------*/
int XMessageBox::Show (const string& text, const string& title, int flags,
                       unsigned int defButton) {
   TRACE1 ("XMessageBox::show (const string&, const string&,"
           " int, unsigned int)\n - Text: " << text << "\nTitle: " << title);

   SmartPtr<XMessageBox> box (new XMessageBox (text, title, flags, defButton));
   Gtk::Main::run ();
   return box->ret;
}
