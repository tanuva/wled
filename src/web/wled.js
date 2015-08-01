var myApp = new Framework7();
var $$ = Dom7; // Export selectors engine

// Framework7 setup
// ================

// Add view
var mainView = myApp.addView('.view-main', {
    // Because we use fixed-through navbar we can enable dynamic navbar
    dynamicNavbar: true
});

// Callbacks to run specific code for specific pages
myApp.onPageInit('hsb', function (page) {
    $$('.create-page').on('click', function () {
        alert("blub")
    });
});

// WLED api calls
// ==============

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

function composeEnabledQuery() {
	return "enabled=1"; // The value actually doesn't matter at all.
}

function post(postData) {
	var async = true;
	var request = new XMLHttpRequest();

	request.onload = function () {
		var status = request.status; // HTTP status code
		var data = request.responseText; // HTTP body
		if (data.result != "ok") {
			console.debug(data)
		}
	}

	console.debug(postData)
	// request.open("POST", document.URL, async);
	request.open("POST", "http://wled/cgi-bin/wled.cgi", async);
	request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
	request.send(postData);
}

r.onchange = function() { post(composeColorQuery()) };
g.onchange = function() { post(composeColorQuery()) };
b.onchange = function() { post(composeColorQuery()) };
enabled.onclick = function() { post(composeEnabledQuery()) };
