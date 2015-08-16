//
//  FirstViewController.swift
//  WLED
//
//  Created by Marcel on 23.07.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import UIKit

class RGBViewController: UIViewController {
	let api: API
	@IBOutlet weak var red: UISlider!
	@IBOutlet weak var green: UISlider!
	@IBOutlet weak var blue: UISlider!

	required init?(coder aDecoder: NSCoder) {
		self.api = API(cgiUrl: Settings.instance().cgiUrl)
		super.init(coder: aDecoder)
	}

	override func viewDidLoad() {
		super.viewDidLoad()
	}

	override func viewWillAppear(animated: Bool) {
		self.api.getColor { (error: APIError?, color: UIColor?) -> Void in
			if let e = error {
				switch e.code {
				case .NETWORK_ERROR:
					print("Network error: \(e.description!)")
					dispatch_sync(dispatch_get_main_queue()) { () -> Void in
						Util.showAlert("Network Error", message: e.description! + "\n(\(self.api.cgiUrl))", parent: self)
					}
					break
				case .JSON_ERROR:
					print("JSON error: \(e.description!)")
					dispatch_sync(dispatch_get_main_queue()) { () -> Void in
						Util.showAlert("Protocol Error", message: e.description! + "\n(\(self.api.cgiUrl))", parent: self)
					}
					break
				}
			} else {
				// No error, color must be valid
				var floats = Array<CGFloat>()
				for _ in 0..<4 {
					floats.append(CGFloat())
				}
				color?.getRed(&floats[0], green: &floats[1], blue: &floats[2], alpha: &floats[3])
				self.red.value   = Float(floats[0]) * 255.0
				self.green.value = Float(floats[1]) * 255.0
				self.blue.value  = Float(floats[2]) * 255.0
				self.updateBackgroundColor(color)
			}
		}
	}

	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}

	func updateBackgroundColor(color: UIColor?) {
		if let c = color {
			self.view.backgroundColor = c;
		} else {
		self.view.backgroundColor = UIColor(colorLiteralRed: self.red.value,
			green: self.green.value,
			blue: self.blue.value,
			alpha: 1.0)
		}
		print("\(self.view.backgroundColor)")
	}

	@IBAction func onEnabledClicked(sender: AnyObject) {
		api.setEnabled(true) { (error: APIError?) -> Void in
			if let e = error {
				switch e.code {
				case .NETWORK_ERROR:
					print("Network error: \(e.description!)")
					dispatch_sync(dispatch_get_main_queue()) { () -> Void in
						Util.showAlert("Network Error", message: e.description! + "\n(\(self.api.cgiUrl))", parent: self)
					}
					break
				default:
					break
				}
			}
		}
	}

	@IBAction func onSliderValueChanged(sender: AnyObject) {
		self.updateBackgroundColor(nil)
		// TODO send color change command
	}
}

