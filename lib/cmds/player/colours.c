/* Do not remove the headers from this file! see /USAGE for more info. */

//: PLAYERCOMMAND
//$$see: mode, palette
// USAGE 
//   |   ``colours``
//   |   ``colours <name> <value>``
//   |   ``colours remove <value>``
//
// This command handles customising the colours displayed in the mud -
// ie changing them to suit your tastes
// Some of types items displayed in the mud are allocated a "colour name",
// which is then given a default "colour value"
// eg "say" could be given a value "blue", in which case all "says" would
// appear in blue. If you wish to have them appear in red, you would use:
//
//   |   ``colours say red``
//   |   ``colours say <021>``
//
// You can also use "bold" as a colour, with comma if necessary - eg::
//     ``colour say bold,red``
//
// You can clear your personalised setting, and revert to the mud default, with::
//     ``colours remove say``
//
// Instead of colour names, check the ``palette`` command and use numbers from there,
// from 001 to 255.
//
// .. TAGS: RST

inherit CMD;
inherit M_COLOURS;

void do_print(string which)
{
   string *colours;
   string *illegal;
   string *irrelevant;
   mapping null = ANSI_D->query_translations()[1];
   int longest;

   if (which == "help")
   {
      write("USAGE:\n");
      write("To change a value:\n\t\"colours <name> <value>\"\n");
      write("For all says to appear in magenta:\n\t\"colours say magenta\" \n");
      write("Find colours on 'palette' cmd:\n\t\"colours say 216\"\n");
      write("To use default value:\n\t\"colours remove <name>\"\n");
      return;
   }

   if (wizardp(this_user()))
      irrelevant = ({});
   else
      irrelevant = ANSI_D->query_restrictions();

   if (which)
      colours = ({which});
   else
      colours = this_user()->query_colours() - irrelevant;

   if (!colours)
   {
      write("No colours set.\n");
      return;
   }

   illegal = filter(colours, ( : $(null)[$1] == 0 :));
   colours -= illegal;

   foreach (string item in colours)
      if (strlen(item) > longest)
         longest = strlen(item);

   illegal = map(illegal, ( : lower_case:));
   if (sizeof(illegal))
      write("Unsupported settings (use 'colours remove whatever'): " + implode(illegal, "\n") + "\n\n");

   colours = map(
       sort_array(colours, 1),
       function(string item, int longest) {
          string what = this_user()->query_colour(item);
          int what_int = to_int(what);
          if (what_int > 0 && strlen(what) == 3 && what_int < 256)
             what = "<" + what + ">" + what + "<res>";
          if (what)
             return sprintf("%-" + longest + "s : %%^%s%%^%s%%^RESET%%^", lower_case(item), upper_case(item), what);
          else
             return sprintf("%-" + longest + "s : (not set)", item);
       },
       longest);
   write(colour_table(colours, this_user()->query_screen_width()) + "\n");
   write("(Use 'colours help' for quick help)\n");
}

nomask private void main(string str)
{
   string which, what;
   string *ansi_names = ({"bold", "green", "yellow", "red", "blue", "magenta", "cyan", "none"});
   int what_int;

   if (!str)
   {
      do_print(0);
      return;
   }

   if (sscanf(str, "%s %s", which, what) != 2)
   {
      do_print(str);
      return;
   }

   if (which == "remove")
   {
      this_user()->remove_colour(what);
      printf("Setting for '%s' removed.\n", what);
      return;
   }

   what_int = to_int(what);
   if (XTERM256_D->colour_code(what) == 0 && member_array(what, ansi_names) == -1)
   {
      printf("Illlegal colour value %s, check colour 'palette' for options.\n", what);
      return;
   }

   this_user()->set_colour(which, what);
   printf("'%s' set to '%%^%s%%^%s%%^RESET%%^'.\n", which, upper_case(which), what);
}
