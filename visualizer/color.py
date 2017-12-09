import random


class Color:
    # Adam's constants
    LUM_FACTOR_R = 0.4
    LUM_FACTOR_G = 0.5
    LUM_FACTOR_B = 0.1

    # Chris's constants
    #LUM_FACTOR_R = 0.2126
    #LUM_FACTOR_G = 0.7152
    #LUM_FACTOR_B = 0.0722

    def __init__(self):
        self.red = None
        self.green = None
        self.blue = None
        self.hue = None
        self.HSLsat = None
        self.HSVsat = None
        self.value = None
        self.lightness = None
        self.luminosity = None

    def randomize(self):
        self.red = random.randint(0, 255)
        self.green = random.randint(0, 255)
        self.blue = random.randint(0, 255)

        # Normalize the RGB for calculations
        normRed = self.red / 255
        normGreen = self.green / 255
        normBlue = self.blue / 255

        # Calculate delta
        RGBmax = max(normRed, normGreen, normBlue)
        RGBmin = min(normRed, normGreen, normBlue)
        delta = RGBmax - RGBmin

        # Calculate lightnessness
        self.lightness = (RGBmax+RGBmin) / 2

        # Calculate saturation and hue
        if delta == 0:
            self.hue = 0 # this may create some strange edge cases
            self.HSLsat = 0
            self.HSVsat = 0
        else:
            self.HSLsat = delta / (1- abs(2*self.lightness - 1))
            self.HSVsat = delta / RGBmax

            if RGBmax == normRed:
                self.hue = (normGreen - normBlue) / delta
            elif RGBmax == normGreen:
                self.hue = ((normBlue - normRed) / delta) + 2
            else:
                self.hue = ((normRed - normGreen) / delta) + 4

            # Hue in degrees
            self.hue *= 60
            if self.hue < 0:
                self.hue += 360

        # Photometric luminosityinosity
        self.luminosity = 0.2126*normRed + 0.7152*normGreen + 0.0722*normBlue
        # Value is max
        self.value = RGBmax

        # %-ize and round all of 'em
        self.hue = round(self.hue, 3)
        self.HSLsat = round(self.HSLsat*100, 3)
        self.HSVsat = round(self.HSVsat*100, 3)
        self.lightness = round(self.lightness*100, 3)
        self.luminosity = round(self.luminosity*100, 3)
        self.value = round(self.value*100, 3)

    def __lt__(self, other):
        if self.red < other.red: return True
        elif self.red > other.red: return False
        elif self.green < other.green: return True
        elif self.green > other.green: return False
        elif self.blue < other.blue: return True
        elif self.blue > other.blue: return False
        return False

    def printinfo(self):
        print("Color info:")
        print("R: ", self.red,"G: ", self.green, "B: ", self.blue)
        print("Hue: ", self.hue)
        print("HSL Saturation: ", self.HSLsat)
        print("HSV Saturation: ", self.HSVsat)
        print("Lightness: ", self.lightness)
        print("Value: ", self.value)
        print("Luminosity: ", self.luminosity)
