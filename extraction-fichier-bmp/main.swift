#! /usr/bin/swift

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

import Foundation

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

func read8 (_ inData : Data, _ ioIndex : inout Int) -> UInt8 {
  let result = inData [ioIndex]
  ioIndex += 1
  return result
}
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

func read16 (_ inData : Data, _ ioIndex : inout Int) -> UInt16 {
  var result = UInt16 (inData [ioIndex + 1])
  result <<= 8
  result |= UInt16 (inData [ioIndex])
  ioIndex += 2
  return result
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

func read32 (_ inData : Data, _ ioIndex : inout Int) -> UInt32 {
  var result = UInt32 (inData [ioIndex + 3])
  result <<= 8
  result |= UInt32 (inData [ioIndex + 2])
  result <<= 8
  result |= UInt32 (inData [ioIndex + 1])
  result <<= 8
  result |= UInt32 (inData [ioIndex])
  ioIndex += 4
  return result
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   MAIN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

  let scriptDirectory = URL (fileURLWithPath: CommandLine.arguments [0]).deletingLastPathComponent ()
  print ("scriptDirectory : \(scriptDirectory)")

  let fichierBMP = scriptDirectory.appendingPathComponent ("LogoECN.bmp")
  let fichierCPP = scriptDirectory.appendingPathComponent ("SplashScreen.cpp")
  let fichierH   = scriptDirectory.appendingPathComponent ("SplashScreen.h")

  let data : Data = try! Data (contentsOf: fichierBMP)

  print ("Taille fichier : \(data.count) octets")

  var idx = 0
//--- Vérifier la marque
  let mark = read16 (data, &idx)
  if mark != 0x4D42 {
    print ("Marque invalide : \(String (mark, radix: 16))")
    exit (1)
  }
//--- Passer 8 octets
  idx += 8
//--- lire "seekOffset"
  let seekOffset = read32 (data, &idx)
  print ("seekOffset : \(seekOffset)")
//--- Passer 4 octets
  idx += 4
//--- lire "width"
  let width = read32 (data, &idx)
  print ("width : \(width)")
//--- lire "height"
  let height = read32 (data, &idx)
  print ("height : \(height)")
//--- Lire valeur réservées
  let ok = (read16 (data, &idx) == 1) && (read16(data, &idx) == 24) && (read32(data, &idx) == 0)
  if !ok {
    print ("Valeurs réservées incorrectes")
    exit (1)
  }
//--- Construire le dictionaire des couleurs
  var colorSet = Set <UInt16> ()
  idx = Int (seekOffset)
  for _ in 0 ..< height {
    for _ in 0 ..< width {
      let red = UInt16 (read8 (data, &idx))
      let green = UInt16 (read8 (data, &idx))
      let blue = UInt16 (read8 (data, &idx))
      let color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)
      colorSet.insert (color)
    }
  }
  print ("\(colorSet.count) couleurs")
//--- Construire les fichiers destination
  var h = "#include <Arduino.h>\n\n"
  h += "static const uint8_t COLOR_COUNT = \(colorSet.count) ;\n"
  h += "extern const uint16_t colorTable [COLOR_COUNT] ;\n\n"
  h += "static const uint16_t SPLASH_SCREEN_WIDTH  = \(width) ;\n"
  h += "static const uint16_t SPLASH_SCREEN_HEIGHT = \(height) ;\n\n"
  h += "extern const uint8_t splashScreen [SPLASH_SCREEN_WIDTH * SPLASH_SCREEN_HEIGHT] ;\n\n"
  var cpp = "#include \"SplashScreen.h\"\n\n"
  var colorDictionary = [UInt16 : UInt8] ()
  var colorIndex : UInt8 = 0
  cpp += "const uint16_t colorTable [COLOR_COUNT] {\n"
  for color in Array (colorSet).sorted () {
    cpp += " 0x\(String (color, radix: 16)), // \(colorIndex)\n"
    colorDictionary [color] = colorIndex
    colorIndex += 1 ;
  }
  cpp += "} ;\n\n"
  cpp += "const uint8_t splashScreen [SPLASH_SCREEN_WIDTH * SPLASH_SCREEN_HEIGHT] = {\n"
//--- Lire l'image
  idx = Int (seekOffset)
  for row in 0 ..< height {
    cpp += "// Ligne \(row)\n"
    var column = 0
    for _ in 0 ..< width {
      let red = UInt16 (read8 (data, &idx))
      let green = UInt16 (read8 (data, &idx))
      let blue = UInt16 (read8 (data, &idx))
      let color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)
      cpp += " \(colorDictionary [color]!),"
      column += 1
      if column == 16 {
        column = 0
        cpp += "\n"
      }
    }
  }
  cpp += "} ;\n\n"
  try! cpp.write (to: fichierCPP, atomically: true, encoding: .ascii)
  try! h.write (to: fichierH, atomically: true, encoding: .ascii)

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
