//
//  SettingsViewController.swift
//  WLED
//
//  Created by Marcel on 23.07.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import Foundation
import UIKit

// Singleton that fetches settings from wled.cgi on init and caches them.
class Settings: NSObject {
	var cgiUrl: String
	static var settings: Settings?

	required override init() {
		// TODO read settings from serialized storage?
		self.cgiUrl = "http://wled/cgi-bin/wled.cgi"
	}

	static func instance() -> Settings {
		if let s = settings {
			return s
		} else {
			self.settings = Settings()
			return self.settings!
		}
	}
}

class SettingsViewController : UITableViewController, UITextFieldDelegate {
	@IBOutlet weak var textFieldCgiUrl: UITextField!

	override func viewDidLoad() {
		super.viewDidLoad()
		textFieldCgiUrl.becomeFirstResponder()
	}

	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}

	@IBAction func btnDone_touchUpInside(sender: AnyObject) {
		self.view.endEditing(false)
		storeSettingsAndDismiss()
	}

	func textFieldShouldReturn(textField: UITextField) -> Bool {
		if(textField == textFieldCgiUrl) {
			storeSettingsAndDismiss()
			return false
		} else {
			print("Oops. Unexpected text field!")
			return false
		}
	}

	func storeSettingsAndDismiss() {
		Settings.instance().cgiUrl = textFieldCgiUrl.text!
		self.dismissViewControllerAnimated(true, completion: nil)
	}
}