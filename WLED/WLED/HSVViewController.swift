//
//  SecondViewController.swift
//  WLED
//
//  Created by Marcel on 23.07.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import UIKit

class HSVViewController: UIViewController {
	let api: API
	@IBOutlet weak var hue: UISlider!
	@IBOutlet weak var saturation: UISlider!
	@IBOutlet weak var value: UISlider!

	required init?(coder aDecoder: NSCoder) {
		self.api = API(cgiUrl: Settings.instance().cgiUrl)
		super.init(coder: aDecoder)
	}

	override func viewDidLoad() {
		super.viewDidLoad()
		// Do any additional setup after loading the view, typically from a nib.
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
				color?.getHue(&floats[0], saturation: &floats[1], brightness: &floats[2], alpha: &floats[3])
				self.hue.value = Float(floats[0])
				self.saturation.value = Float(floats[1])
				self.value.value = Float(floats[2])
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
			self.view.backgroundColor = UIColor(hue: CGFloat(self.hue.value),
				saturation: CGFloat(self.saturation.value),
				brightness: CGFloat(self.value.value),
				alpha: 1.0)
		}
		print("\(self.view.backgroundColor)")
	}

	@IBAction func onSliderValueChanged(sender: UISlider) {
		let color = UIColor(hue: CGFloat(self.hue.value),
			saturation: CGFloat(self.saturation.value),
			brightness: CGFloat(self.value.value),
			alpha: 1.0)
		self.updateBackgroundColor(color)
		self.api.setColor(color) { (error: APIError?) -> Void in
			// TODO Handle result+
		}
	}

	// TODO Move implementation to new class CommonActions
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
}

