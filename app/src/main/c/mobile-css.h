/* Fixed stylesheet only: no extraction, credential handling or JS bridge. */
static const char mobile_script[] =
    "(function(){var h=document.head||document.documentElement;"
    "if(!document.querySelector('meta[name=viewport]')){"
    "var m=document.createElement('meta');m.name='viewport';"
    "m.content='width=device-width,initial-scale=1';h.appendChild(m);}"
    "if(!document.getElementById('ze-mobile')){"
    "var s=document.createElement('style');s.id='ze-mobile';"
    "s.textContent='html,body{max-width:100%!important;overflow-x:hidden!important}'"
    "+'input,select,textarea,button{font-size:16px!important;min-height:44px!important}'"
    "+'table{max-width:100%!important;display:block!important;overflow-x:auto!important}'"
    "+'img{max-width:100%!important;height:auto!important}';h.appendChild(s);}"
    "})();";
