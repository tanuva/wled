//
//  API.swift
//  WLED
//
//  Created by Marcel on 23.07.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import Foundation
import UIKit

enum APIErrorCode: ErrorType {
	case NETWORK_ERROR
	case JSON_ERROR
}

class APIError {
	let code: APIErrorCode
	var description: String?

	required init(code: APIErrorCode, description: String?) {
		self.code = code
		self.description = description
	}
}

class API: NSObject {
	let cgiUrl: String
	let session: NSURLSession

	required init(cgiUrl: String) {
		self.cgiUrl = cgiUrl
		let urlSessionConfig = NSURLSessionConfiguration.ephemeralSessionConfiguration()
		self.session = NSURLSession(configuration: urlSessionConfig)
	}

	func setEnabled(enabled: Bool, handler: (APIError?) -> Void) {
		self.sendPost("enabled=1", handler: handler)
	}

	func getColor(handler: (APIError?, UIColor?) -> Void) {
		let request = NSURLRequest(URL: NSURL(string: self.cgiUrl + "/color")!)
		print("<< \(request.URL!)")
		let task = self.session.dataTaskWithRequest(request) {
			(data: NSData?, response: NSURLResponse?, error: NSError?) -> Void in
			// Example:
			// { "color": "3eff18" }

			if let e = error {
				print("Error: \(e.localizedDescription)\nReason: \(e.localizedFailureReason)")
				handler(APIError(code: APIErrorCode.NETWORK_ERROR, description: e.localizedDescription), nil)
				return
			}
			// TODO Are we interested in 'response'?

			let jsonString = String(NSString(data: data!, encoding: NSUTF8StringEncoding)!)
			do {
				print(">> \(jsonString)")
				let parsedObject = try NSJSONSerialization.JSONObjectWithData(data!, options: NSJSONReadingOptions.AllowFragments)
				guard let dict = parsedObject as? NSDictionary else {
					throw APIErrorCode.JSON_ERROR
				}
				handler(nil, API.colorWithHexString(dict["color"] as! String))
			} catch _ {
				handler(APIError(code: APIErrorCode.JSON_ERROR, description: jsonString), nil)
			}
		}
		task.resume()
	}

	func setColor(color: UIColor, handler: (APIError?) -> Void) {
		print("sending " + API.hexStringWithColor(color))
		sendPost("color=" + API.hexStringWithColor(color), handler: handler)
	}

	func sendPost(data: String, handler: (APIError?) -> Void) {
		let request = NSMutableURLRequest(URL: NSURL(string: self.cgiUrl)!)
		request.setValue("text/plain", forHTTPHeaderField: "Content-Type")
		request.HTTPMethod = "POST"
		request.HTTPBody = data.dataUsingEncoding(NSUTF8StringEncoding)
		print("<< \(request.URL!) Body: \(data)")
		let task = self.session.dataTaskWithRequest(request) { (data: NSData?, response: NSURLResponse?, error: NSError?) -> Void in
			if let e = error {
				print("Error: \(e.localizedDescription)\nReason: \(e.localizedFailureReason)")
				handler(APIError(code: APIErrorCode.NETWORK_ERROR, description: e.localizedDescription))
				return
			}
			if let r = (response as? NSHTTPURLResponse) {
				if r.statusCode != 200 {
					handler(APIError(code: APIErrorCode.NETWORK_ERROR, description: "Code: \(r.statusCode)\nDescription: \(r.description)"))
					return
				}
			}
			print(">> \(String(NSString(data: data!, encoding: NSUTF8StringEncoding)!))")
			handler(nil)
		}
		task.resume()
	}

	// Creates a UIColor from a hex string. (Adapted from: arshad, https://gist.github.com/arshad/de147c42d7b3063ef7bc)
	static func colorWithHexString(hex: String) -> UIColor {
		var tmpHex = hex.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet()).uppercaseString

		if tmpHex.hasPrefix("#") {
			tmpHex = tmpHex.substringFromIndex(tmpHex.startIndex.advancedBy(1))
		}

		if (tmpHex.characters.count != 6) {
			return UIColor.grayColor()
		}

		let rString = tmpHex.substringToIndex(tmpHex.startIndex.advancedBy(2))
		let gString = tmpHex.substringFromIndex(tmpHex.startIndex.advancedBy(2)).substringToIndex(tmpHex.startIndex.advancedBy(2))
		let bString = tmpHex.substringFromIndex(tmpHex.startIndex.advancedBy(4))

		var r:CUnsignedInt = 0, g:CUnsignedInt = 0, b:CUnsignedInt = 0;
		NSScanner(string: rString).scanHexInt(&r)
		NSScanner(string: gString).scanHexInt(&g)
		NSScanner(string: bString).scanHexInt(&b)
		return UIColor(red: CGFloat(Float(r) / 255.0),
			green: CGFloat(Float(g) / 255.0),
			blue:  CGFloat(Float(b) / 255.0),
			alpha: CGFloat(1))
	}

	// Creates a hex string from a UIColor. (Adapted from http://stackoverflow.com/a/14428870)
	static func hexStringWithColor(color: UIColor) -> String {
		var hexColor = ""

		// This method only works for RGBA colors
		if (CGColorGetNumberOfComponents(color.CGColor) == 4) {
			let components = CGColorGetComponents(color.CGColor)
			let r = roundf(Float(components[0] * 255.0))
			let g = roundf(Float(components[1] * 255.0))
			let b = roundf(Float(components[2] * 255.0))

			// Convert with %02x (use 02 to always get two chars)
			hexColor = String(NSString(format: "%02x%02x%02x", Int(r), Int(g), Int(b)))
		}

		return hexColor;
	}
}
