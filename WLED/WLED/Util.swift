//
//  Util.swift
//  WLED
//
//  Created by Marcel on 16.08.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import Foundation
import UIKit

class Util {
	static func showAlert(title: String, message: String, parent: UIViewController) {
		let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
		alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.Default, handler: nil))
		parent.presentViewController(alert, animated: true, completion: nil)
	}

	static func handleError(error: APIError, api: API, parent: UIViewController) {
		switch error.code {
		case .NETWORK_ERROR:
			print("Network error: \(error.description!)")
			dispatch_sync(dispatch_get_main_queue(), { () -> Void in
				Util.showAlert("Network Error", message: error.description! + "\n(\(api.cgiUrl))", parent: parent)
			})
			break
		case .JSON_ERROR:
			print("JSON error: \(error.description!)")
			dispatch_sync(dispatch_get_main_queue(), { () -> Void in
				Util.showAlert("Protocol Error", message: error.description! + "\n(\(api.cgiUrl))", parent: parent)
			})
			break
		}
	}
}
