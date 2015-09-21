//
//  GradientSlider.swift
//  WLED
//
//  Created by Marcel on 16.08.15.
//  Copyright © 2015 Marcel Brüggebors. All rights reserved.
//

import Foundation
import UIKit

class GradientSlider : UISlider {
	func setTrackGradient(colors: [UIColor]) {
		// Source: http://iostricks-ipreencekmr.blogspot.de/2013/07/setting-gradient-colors-to-uislider-bar.html
		let view = self.subviews[0]
		let maxTrackImageView = view.subviews[0]

		// Configure the gradient layer
		let gradientLayer = CAGradientLayer()
		gradientLayer.colors = [UIColor.whiteColor().CGColor, UIColor.grayColor().CGColor, UIColor.blackColor().CGColor]
		gradientLayer.startPoint = CGPoint(x: 0, y: 0.5)
		gradientLayer.endPoint = CGPoint(x: 1, y: 0.5)
		var rect = maxTrackImageView.frame
		rect.origin.x = view.frame.origin.x // Wtf?
		gradientLayer.frame = rect

		maxTrackImageView.layer.insertSublayer(gradientLayer, atIndex: 0)
	}
}
