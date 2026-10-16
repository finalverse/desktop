// Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
// Render a labelled still-image recap. This never simulates a live screen recording.
import AVFoundation
import CoreGraphics
import CoreText
import ImageIO
import Foundation

guard CommandLine.arguments.count == 4 else {
    fatalError("Usage: swift demo_recap.swift built.png undone.png output.mp4")
}
let output = URL(fileURLWithPath: CommandLine.arguments[3])
guard !FileManager.default.fileExists(atPath: output.path) else {
    fatalError("Refusing to replace existing demo media")
}
let images = try CommandLine.arguments[1...2].map { path -> CGImage in
    guard let source = CGImageSourceCreateWithURL(URL(fileURLWithPath: path) as CFURL, nil),
          let image = CGImageSourceCreateImageAtIndex(source, 0, nil) else {
        throw NSError(domain: "FinalverseRecap", code: 1)
    }
    return image
}
let width = 1280, height = 850, fps: Int32 = 12
let writer = try AVAssetWriter(outputURL: output, fileType: .mp4)
let input = AVAssetWriterInput(mediaType: .video, outputSettings: [
    AVVideoCodecKey: AVVideoCodecType.h264, AVVideoWidthKey: width,
    AVVideoHeightKey: height, AVVideoCompressionPropertiesKey: [
        AVVideoAverageBitRateKey: 1_000_000, AVVideoProfileLevelKey: AVVideoProfileLevelH264HighAutoLevel
    ]
])
input.expectsMediaDataInRealTime = false
let adaptor = AVAssetWriterInputPixelBufferAdaptor(assetWriterInput: input,
    sourcePixelBufferAttributes: [kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32ARGB,
                                kCVPixelBufferWidthKey as String: width,
                                kCVPixelBufferHeightKey as String: height])
writer.add(input)
guard writer.startWriting() else { throw writer.error! }
writer.startSession(atSourceTime: .zero)
func text(_ string: String, at point: CGPoint, size: CGFloat, in context: CGContext) {
    let attributes = [kCTFontAttributeName: CTFontCreateWithName("Helvetica" as CFString, size, nil),
                      kCTForegroundColorAttributeName: CGColor(gray: 1, alpha: 1)] as CFDictionary
    let line = CTLineCreateWithAttributedString(CFAttributedStringCreate(nil, string as CFString, attributes))
    context.textPosition = point
    CTLineDraw(line, context)
}
for index in 0..<(20 * Int(fps)) {
    let second = Double(index) / Double(fps)
    while !input.isReadyForMoreMediaData {
        guard writer.status == .writing else { throw writer.error! }
        Thread.sleep(forTimeInterval: 0.01)
    }
    var buffer: CVPixelBuffer?
    guard CVPixelBufferPoolCreatePixelBuffer(nil, adaptor.pixelBufferPool!, &buffer) == kCVReturnSuccess,
          let pixel = buffer else { fatalError("Pixel buffer allocation failed") }
    CVPixelBufferLockBaseAddress(pixel, [])
    let context = CGContext(data: CVPixelBufferGetBaseAddress(pixel), width: width, height: height,
        bitsPerComponent: 8, bytesPerRow: CVPixelBufferGetBytesPerRow(pixel),
        space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: CGImageAlphaInfo.noneSkipFirst.rawValue)!
    context.setFillColor(CGColor(red: 0.035, green: 0.055, blue: 0.085, alpha: 1))
    context.fill(CGRect(x: 0, y: 0, width: width, height: height))
    let built = second < 10
    let image = images[built ? 0 : 1]
    let scale = min(1240 / CGFloat(image.width), 710 / CGFloat(image.height))
    let w = CGFloat(image.width) * scale, h = CGFloat(image.height) * scale
    context.draw(image, in: CGRect(x: (CGFloat(width) - w) / 2, y: 76 + (710 - h) / 2, width: w, height: h))
    text("FINALVERSE × MUTSEA  /  HOSTED LUMI", at: CGPoint(x: 24, y: 810), size: 21, in: context)
    text(built ? "Reviewed approval → 147 native parts built" : "Undo creation → 423-part baseline restored",
         at: CGPoint(x: 24, y: 47), size: 23, in: context)
    text("15 Oct 2026 · Captioned still-image recap of actual Mac tests · Not a continuous recording",
         at: CGPoint(x: 24, y: 20), size: 15, in: context)
    CVPixelBufferUnlockBaseAddress(pixel, [])
    guard adaptor.append(pixel, withPresentationTime: CMTime(value: Int64(index), timescale: fps)) else {
        throw writer.error!
    }
}
input.markAsFinished()
let finished = DispatchSemaphore(value: 0)
writer.finishWriting { finished.signal() }
finished.wait()
guard writer.status == .completed else { throw writer.error! }
print("Rendered 20-second captioned native still-image recap: \(output.lastPathComponent)")
