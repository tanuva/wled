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
	@IBOutlet weak var red: GradientSlider!
	@IBOutlet weak var green: GradientSlider!
	@IBOutlet weak var blue: GradientSlider!

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
				Util.handleError(e, api: self.api, parent: self)
			} else {
				// No error, color must be valid
				var cgRed   = CGFloat()
				var cgGreen = CGFloat()
				var cgBlue  = CGFloat()
				var cgAlpha = CGFloat()
				color?.getRed(&cgRed, green: &cgGreen, blue: &cgBlue, alpha: &cgAlpha)
				self.red.value   = Float(cgRed)
				self.green.value = Float(cgGreen)
				self.blue.value  = Float(cgBlue)
				self.updateBackgroundColor(color)
			}
		}
	}

	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}

	func updateBackgroundColor(color: UIColor?) {
		print("Setting bg color")
		if let c = color {
			self.view.backgroundColor = c;
		} else {
			self.view.backgroundColor = UIColor(red: CGFloat(self.red.value),
				green: CGFloat(self.green.value),
				blue: CGFloat(self.blue.value),
				alpha: CGFloat(1.0))
		}
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
		let color = UIColor(red: CGFloat(self.red.value),
			green: CGFloat(self.green.value),
			blue: CGFloat(self.blue.value),
			alpha: CGFloat(1.0))
		self.updateBackgroundColor(color)
		self.api.setColor(color) { (error: APIError?) -> Void in
			if let e = error {
				Util.handleError(e, api: self.api, parent: self)
			}
		}
	}
}

