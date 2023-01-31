static const char *dialog_download = "\
{ \n\
 \"info\": {\"version\": 1, \"license\": \"LGPL-2.0-or-later\", \"author\": \"Cyril Hrubis <metan@ucw.cz>\"}, \n\
 \"layout\": { \n\
  \"widgets\": [ \n\
   {\"type\": \"frame\", \"uid\": \"title\", \"title\": \"Download\", \n\
    \"widget\": { \n\
     \"rows\": 3, \n\
     \"widgets\": [ \n\
      { \n\
       \"cols\": 2, \n\
       \"align\": \"hfill\", \n\
       \"widgets\": [ \n\
        {\"type\": \"stock\", \"stock\": \"arrow_down\"}, \n\
        {\"type\": \"label\", \"text\": \"filename\", \"uid\": \"filename\"} \n\
       ] \n\
      }, \n\
      {\"type\": \"pbar\", \"uid\": \"progress\", \"align\": \"hfill\"}, \n\
      {\"type\": \"button\", \"label\": \"Cancel\", \"on_event\": \"cancel\", \"focused\": true} \n\
     ] \n\
    } \n\
   } \n\
  ] \n\
 } \n\
} \n\
";
