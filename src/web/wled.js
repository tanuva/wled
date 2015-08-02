
// WLED api calls
// ==============
const API = {
	"cgiUrl": "http://wled/cgi-bin/wled.cgi",

	// Converts a hex string (AABBCC) to separate RGB integers.
	"hexToRgb": function(hex) {
	    var bigint = parseInt(hex, 16);
	    var r = (bigint >> 16) & 255;
	    var g = (bigint >> 8) & 255;
	    var b = bigint & 255;
	    return { "r": r, "g": g, "b": b };
	},

	"rgbToHex": function(rgb) {
		var convert = function(val) {
			var result = val.toString(16);
			return result.length == 1 ? "0" + result : result;
		}
		return convert(parseInt(rgb["r"]))
				+ convert(parseInt(rgb["g"]))
				+ convert(parseInt(rgb["b"]));
	},

	"rgbToHsb": function(rgb) {
		const MIN = Math.min(rgb["r"], rgb["g"], rgb["b"]);
		const MAX = Math.max(rgb["r"], rgb["g"], rgb["b"]);

		var hue = 0;
		if (MIN == MAX) hue = 0;
		else if (MAX == rgb["r"]) hue = 60.0 *      (rgb["g"] - rgb["b"]) / (MAX - MIN);
		else if (MAX == rgb["g"]) hue = 60.0 * (2 + (rgb["b"] - rgb["r"]) / (MAX - MIN));
		else if (MAX == rgb["b"]) hue = 60.0 * (4 + (rgb["r"] - rgb["g"]) / (MAX - MIN));

		var saturation = 0;
		if (MAX != 0) saturation = (MAX - MIN) / MAX;
		return { "h": hue, "s": saturation, "b": MAX }
	},

	"hsbToRgb": function(hsb) {
		if (hsb["s"] = 0) {
			// Neutral grey
			return { "r": hsb["b"], "g": hsb["b"], "b": hsb["b"] }
		}

		const hi = Math.floor(hue.value / 60.0);
		const f = hue.value / 60.0 - hi;
		const p = brightness.value * (1 - saturation.value);
		const q = brightness.value * (1 - saturation.value * f);
		const t = brightness.value * (1 - saturation.value * (1 - f));

		var rgb = null;
		switch(hi) {
		case 0:
		case 6:
			rgb = { "r": hsb["b"], "g": t, "b": p }; break;
		case 1:
			rgb = { "r": q, "g": hsb["b"], "b": p }; break;
		case 2:
			rgb = { "r": p, "g": hsb["b"], "b": t }; break;
		case 3:
			rgb = { "r": p, "g": q, "b": hsb["b"] }; break;
		case 4:
			rgb = { "r": t, "g": p, "b": hsb["b"] }; break;
		case 5:
			rgb = { "r": hsb["b"], "g": p, "b": q }; break;
		default:
			console.log("Unexpected case in hsbToRgb: hi = " + hi);
		}

		// Scale [0, 1] to [0, 255]
		rgb["r"] *= 255;
		rgb["g"] *= 255;
		rgb["b"] *= 255;
		return rgb;
	},

	"setColor": function(color) {
		API.post("color=" + API.rgbToHex(color));
	},

	"getColor": function(callback) {
		var async = true;
		var request = new XMLHttpRequest();
		request.onload = function() {
			if (request.status != 200) {
				alert("Unexpected status code: "  + request.status);
			}

			var response = JSON.parse(request.responseText)
			callback(API.hexToRgb(response.color));
		}
		request.open("GET", API.cgiUrl + "/color", async);
		request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
		request.send()
	},

	"setEnabled": function(enabled) {
		const enabledInt = parseInt(enabled)
		if (enabledInt < 0 || enabledInt > 1) {
			console.log("Unexpected enabled value: " + enabledInt);
		}
		API.post("enabled=" + enabledInt);
	},

	"post": function(postData) {
		var async = true;
		var request = new XMLHttpRequest();

		request.onload = function() {
			var status = request.status; // HTTP status code
			var data = JSON.parse(request.responseText); // HTTP body
			if (data.result != "ok") {
				console.debug("Error: " + data)
			}
		}

		request.open("POST", API.cgiUrl, async);
		request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
		request.send(postData);
	},
}

// Framework7 setup
// ================
var myApp = new Framework7({
	// Disable automatic init, prevents onPageInit for index page as the page
	// is already initialized when the callback is registered.
	init: false
});
var $$ = Dom7; // Export selectors engine

// Add view
var mainView = myApp.addView('.view-main', {
    // Because we use fixed-through navbar we can enable dynamic navbar
    dynamicNavbar: true
});

// Callbacks to run specific code for specific pages
// TODO select proper events
myApp.onPageInit("index", function(page) {
	API.getColor(function(rgb) {
		r.value = rgb["r"];
		g.value = rgb["g"];
		b.value = rgb["b"];

		const hsb = API.rgbToHsb(rgb);
		hue.value = hsb["h"];
		saturation.value = hsb["s"];
		brightness.value = hsb["b"];
	});
});

// Don't forget this! Auto init was disabled in the Framework7 ctor.
myApp.init();

var red     = document.getElementById("r");
var green   = document.getElementById("g");
var blue    = document.getElementById("b");
var enabled = document.getElementById("enabled");

function getRgb() {
	return {
		"r": red.value,
		"g": green.value,
		"b": blue.value
	};
}

r.onchange = function() { API.setColor(getRgb()); };
g.onchange = function() { API.setColor(getRgb()); };
b.onchange = function() { API.setColor(getRgb()); };
enabled.onclick = function() { API.setEnabled(1); };

var hue        = document.getElementById("hue");
var saturation = document.getElementById("saturation");
var brightness = document.getElementById("brightness");

function getHsb() {
	return {
		"h": hue.value,
		"s": saturation.value,
		"b": brightness.value
	};
}

hue.onchange = function() { API.setColor(API.hsbToRgb(getHsb())); };
saturation.onchange = function() { API.setColor(API.hsbToRgb(getHsb())); };
brightness.onchange = function() { API.setColor(API.hsbToRgb(getHsb())); };
// enabled.onclick = function() { API.setEnabled(1); };
