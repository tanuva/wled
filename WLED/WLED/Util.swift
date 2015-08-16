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
		parent.presentViewController(alert, animated: false, completion: nil)
	}
}
