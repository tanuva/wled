var r = document.getElementById('r');
var g = document.getElementById('g');
var b = document.getElementById('b');

function composeQuery() {
	var convert = function(val) {
		var result = val.toString(16);
		console.debug(result);
		return result.length == 1 ? "0" + result : result;
	}
	return "set=" + convert(parseInt(r.value))
					+ convert(parseInt(g.value))
					+ convert(parseInt(b.value));
}

function setValues() {
	var postData = composeQuery();
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
	request.open("POST", document.URL, async);
	request.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
	request.send(postData);
}

r.onchange = setValues;
g.onchange = setValues;
b.onchange = setValues;
