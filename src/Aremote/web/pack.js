// https://www.minifier.org/
// https://cssminifier.com/
// https://javascript-minifier.com/

    var AREMOTE = (function(module) {
         "use strict";

	module.KeyBoardLayout = function(arg) {
	   var self    = this;
	   self.akbd   = document.getElementById(arg);
	   self.layoutK = {
	     map_default: [
               "{power} {standby} {home} {escape} {app} {back} {menu} {alarm} {wakeup} {sleep} {soft sleep} {left} {right} {up} {down} {pageup} {pagedown} {center}",
	       "{F1} {F2} {F3} {F4} {F5} {F6} {F7} {F8} {F9} {F10} {F11} {F12}",
	       "1 2 3 4 5 6 7 8 9 0 @ + - = [ ] \\ ; ' , . / ` *",
               "{tab} {language} q w e r t y u i o p a s d f g h j k l z x c v b n m",
               "{L shift} {L alt} {L thumb} {caps lock} {num lock} {scroll lock} {space} {enter} {delete} {R thumb} {R alt} {R shift}",
               "{www} {mail} {search} {camera} {screenshot} {voice assist} {music} {Phone call} {Phone end call}",
               "{play>/pause} {stop} {next>>} {<<prev} {rewind} {forward} {mute} {volume mute} {volume down} {volume up}",
               "{media play} {media pause} {media stop} {media eject} {media record} {media track} {media menu} {skip forward} {skip backward} {step forward} {step backward}",
               "{BT play} {HS play>/pause} {BT pause} {BT stop} {BT next} {BT prev} {BT rewind} {BT forward} {HS hook} {HS end call} {HS mute} {HS volume down} {HS volume up}",
               "{cut} {copy} {paste} {insert} {delete from end} {cursor to begin} {cursor to end} {brightness up} {brightness down}"
	     ]
	   };
	   self.layoutV = {
	     map_default: [
	       [26, 6, 3, 111, 187, 4, 82, 114, 224, 223, 276, 21, 22, 19, 20, 92, 93, 23],
	       [131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142],
	       [8, 9, 10, 11, 12, 13, 14, 15, 16, 7, 77, 81, 69, 70, 71, 72, 73, 74, 75, 55, 56, 76, 68, 18, 17],
	       [61, 63, 45, 51, 33, 46, 48, 53, 49, 37, 43, 44, 29, 47, 32, 34, 35, 36, 38, 39, 40, 54, 52, 31, 50, 30, 42, 41],
	       [59, 57, 106, 115, 143, 116, 62, 66, 67, 107, 58, 60],
	       [64, 65, 84, 27, 120, 231, 209, 5, 6],
	       [85, 86, 87, 88, 89, 90, 91, 164, 25, 24],
	       [126, 127, 128, 129, 130, 222, 226, 272, 273, 274, 275],
	       [200, 164, 201, 166, 163, 165, 122, 168, 208, 226, 107, 113, 114, 115],
	       [277, 278, 279, 124, 112, 122, 123, 221, 220]
	     ]
	   };
	   //
	   function set(id) {
	      if (!self.akbd)
		 return;
	      while(self.akbd.firstChild) {
	        self.akbd.removeChild(self.akbd.firstChild);
	      }
	      var span = document.createElement('span');
	      for (var i = 0; i < self.layoutK[id].length; i++) {
	        //
	        var idx = self.layoutV[id][i];
	        var str = self.layoutK[id][i];
	        var div = document.createElement('div');
		div.classList.add('wakbd-row');
	        //
	        for (var n = 0, m = 0; n < str.length; n++) {
		  var c = str.charAt(n);
		  if (c === ' ')
		    continue;
		  if (c === '{')
		    for (var k = n + 1; k < str.length; k++) {
		      var e = str.charAt(k);
		      if (e === '}') {
		        c = str.substring(n, (k + 1));
		        n = (k + 1);
		        break;
		      }
		      if (e === '{')
		        break;
		    }
		  var btn = document.createElement('button');
		  btn.classList.add('wakbd-kbdbtn');
		  btn.appendChild(document.createTextNode(c));
		  btn.setAttribute('data-value', idx[m++]);
	          div.appendChild(btn);
		}
	        span.appendChild(div);
	      }
	      self.akbd.appendChild(span);
	   }
	   self.set = function(id) { set(id); }
	};
	//
	module.Control = function(args) {

	   var self         = this;
	   self.kbdlayot    = new module.KeyBoardLayout('akbdbody');
	   self.isrun       = false;
	   self.url         = args.url       || '/stream/live.mjpg?rnd=';
	   self.urlsplash   = args.urlsplash || '/stream/splash.jpg';
	   self.urlratio    = args.urlratio  || 0;
	   self.urlrotate   = args.urlrotate || 0;
	   self.menu        = document.getElementById('css3menu0');
	   self.menukbd     = document.getElementById('makbd');
	   self.mplayer     = document.getElementById('mplayer');
	   self.errorbox    = document.getElementById('errmsg');
	   //
	   self.mplayer.setAttribute('src', self.urlsplash);
	   self.mplayer.focus();
	   self.kbdlayot.set('map_default');
	   //
	   self.menu.onmouseout = function(e) {
	        if (self.isrun)
	           self.mplayer.focus();
	   }
	   self.mplayer.onmouseenter = function(e) {
	        if (self.isrun)
	           self.menukbd.classList.add('pressed');
	   }
	   self.mplayer.onmouseout = function(e) {
	        if (self.isrun)
	           self.menukbd.classList.remove('pressed');
	   }
	   function geturl() {
	        var rnd = Math.floor(Math.random() * Math.pow(2, 31));
		return self.url + rnd + '&ratio=' + self.urlratio + '&rotate=' + self.urlrotate;
	   }
	   function renew() {
	        if (self.isrun)
	        {
		   self.mplayer.setAttribute('src', geturl());
	           self.mplayer.focus();
	        }
	   }
	   function Obj(arg) {
	        return document.getElementById(arg);
	   }
	   function AddEvent(arg, ev, fun) {
	        var obj = Obj(arg);
	        if (!obj)
		    console.log("object not found: ", arg);
	        else
		    obj.addEventListener(ev, function(e) { fun(e); });
	   }
	   function Click(arg, fun) {
	        AddEvent(arg, 'click', function(e) { fun(e); });
	   }
	   function Keys(arg, fun) {
	        AddEvent(arg, 'keyup', function(e) { fun(e); });
	   }
	   function Trigger(arg) {
	        var obj = Obj(arg);
	        if (!obj)
	        {
		    console.log("object not found: ", arg);
		    return;
	        }
	        if (obj.style.display == 'block')
		    obj.style.display = 'none';
	        else
		    obj.style.display = 'block';
	   }
	   function Error(text) {
	        self.errorbox.innerHTML += text + '<br/>';
	        if (self.errorbox.timer)
		    window.clearTimeout(self.errorbox.timer);

	        self.errorbox.timer = window.setTimeout(
		    function() { self.errorbox.innerHTML = ""; }, 5000
	        );
	   }
	   function IsRequestError(arg) {
	        if ((arg.readyState == 4) && (arg.status != 200)) {
		    Error('Request error: ' + arg.status + ' : ' + arg.statusText);
		}
	   }
	   function setevent() {
	        Click('btnstart', function() {
		    self.isrun = true;
		    self.mplayer.setAttribute('src', geturl());
		});
	        Click('btnstop', function() {
		    self.isrun = false;
		    mplayer.setAttribute('src', self.urlsplash);
		});
	        Click('btnrotate0', function() {
		    self.urlrotate = 0;
		    renew();
		});
	        Click('btnrotate90', function() {
		    self.urlrotate = 1;
		    renew();
		});
	        Click('btnrotate180', function() {
		    self.urlrotate = 2;
		    renew();
		});
	        Click('btnrotate270', function() {
		    self.urlrotate = 3;
		    renew();
		});
	        Click('btnratio11', function() {
		    self.urlratio  = 0;
		    renew();
		});
	        Click('btnratio12', function() {
		    self.urlratio  = 1;
		    renew();
		});
	        Click('btnratio13', function() {
		    self.urlratio  = 2;
		    renew();
		});
	        Click('btndefault', function() {
		    self.urlrotate = 0;
		    self.urlratio  = 0;
		    renew();
		});
	        Click('akbdclose', function() {
		    Trigger('akbd');
		});
	        Click('akbd', function(e) {
		    if (!self.isrun)
		        return;

		    try {
		      if (e.originalTarget.attributes.length != 2)
		        return;

		      var code = e.originalTarget.attributes["data-value"];
		      if ((!code) || (!code.value))
		        return;

		      // console.log('/akey/' + code.value, " -> ", code);

		      var xhr = new XMLHttpRequest();
		      xhr.open('GET', '/akey/' + code.value, true);
		      xhr.send();
		      xhr.onreadystatechange = function() { IsRequestError(xhr); };
		    } catch(err) {
		    	  Error(err);
		    }
		});
		//
	        self.menukbd.onclick = function(e) {
		    if (!self.isrun)
		        return;
		    Trigger('akbd');
		};
		//
	        self.mplayer.onclick = function(e) {
		    if (!self.isrun)
		        return;

		    var posX = e.offsetX;
		    var posY = e.offsetY;
		    // console.log("X:", posX, "Y:", posY, " -> ", e.clientX, e.clientY, e.pageX, e.pageY);

		    try {
		      var xhr = new XMLHttpRequest();
		      xhr.open('GET', '/tap/' + posX + '/' + posY + '/' + self.urlrotate + '/' + self.urlratio, true);
		      xhr.send();
		      xhr.onreadystatechange = function() { IsRequestError(xhr); };
		    } catch(err) {
		    	  Error(err);
		    }
		};
	        self.mplayer.addEventListener('keyup', function(e) {
		    if (!self.isrun)
		        return;
		    if (e.ctrlKey || e.altKey || e.metaKey)
			return;

		    try {
		      if (isNaN(e.keyCode))
			return;
		      var xhr = new XMLHttpRequest();
		      xhr.open('GET', '/key/' + e.keyCode + '/2', true);
		      xhr.send();
		      xhr.onreadystatechange = function() { IsRequestError(xhr); };
		    } catch(err) {
		    	  Error(err);
		    }
		});
	        Keys('intxt', function(e) {

		    if ((!self.isrun) || (e.key != 'Enter'))
		        return;

		    var obj = Obj('intxt');
		    var txt = obj.value;
		    if ((txt === "") || (!txt.trim()))
		        return;
                    obj.value = "";

		    try {
		      var xhr = new XMLHttpRequest();
		      xhr.open('POST', '/text', true);
		      xhr.send(txt);
		      xhr.onreadystatechange = function() { IsRequestError(xhr); };
		    } catch(err) {
		    	  Error(err);
		    }
		});
	   };
	   self.init = function() { setevent(); }
	};
        return module;

     })(AREMOTE || {});

     var player = new AREMOTE.Control({});
     player.init();

