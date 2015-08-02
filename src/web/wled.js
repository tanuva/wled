
// WLED api calls
// ==============

const baseUrl = "http://wled";
const cgiUrl = baseUrl + "/cgi-bin/wled.cgi";
var r = document.getElementById('r');
var g = document.getElementById('g');
var b = document.getElementById('b');
var enabled = document.getElementById('enabled');

function composeColorQuery() {
	var convert = function(val) {
		var result = val.toString(16);
		console.debug(result);
		return result.length == 1 ? "0" + result : result;
	}
	return "color=" + convert(parseInt(r.value))
					+ convert(parseInt(g.value))
					+ convert(parseInt(b.value));
}

// Converts a hex string (AABBCC) to separate RGB integers.
function hexToRgb(hex) {
	console.debug(hex);
    var bigint = parseInt(hex, 16);
    var r = (bigint >> 16) & 255;
    var g = (bigint >> 8) & 255;
    var b = bigint & 255;
	console.debug({ "r": r, "g": g, "b": b })

    return { "r": r, "g": g, "b": b };
}

function composeEnabledQuery() {
	return "enabled=1"; // The value actually doesn't matter at all.
}

function fetchCurrentColor(callback) {
	var async = true;
	var request = new XMLHttpRequest();
	request.onload = function() {
		if (request.status != 200) {
			alert("Unexpected status code: "  + request.status);
		}

		var response = JSON.parse(request.responseText)
		callback(hexToRgb(response.color));
	}
	request.open("GET", cgiUrl + "/color", async);
	request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
	request.send()
}

function post(postData) {
	var async = true;
	var request = new XMLHttpRequest();

	request.onload = function() {
		var status = request.status; // HTTP status code
		var data = JSON.parse(request.responseText); // HTTP body
		if (data.result != "ok") {
			console.debug(data)
		}
	}

	// request.open("POST", document.URL, async);
	request.open("POST", cgiUrl, async);
	request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
	request.send(postData);
}

r.onchange = function() { post(composeColorQuery()) };
g.onchange = function() { post(composeColorQuery()) };
b.onchange = function() { post(composeColorQuery()) };
enabled.onclick = function() { post(composeEnabledQuery()) };

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
	fetchCurrentColor(function(color) {
		r.value = color["r"];
		g.value = color["g"];
		b.value = color["b"];
	});
});

// Don't forget this! Auto init was disabled in the Framework7 ctor.
myApp.init();
