from PIL import Image
import sys, colorsys
if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " <image file>")
    exit(1)

img = Image.open(sys.argv[1])
px = img.load()

base_col = px[0, img.height - 1]
base_hsv = colorsys.rgb_to_hsv(base_col[0] / 255, base_col[1] / 255, base_col[2] / 255)

for i in range(img.width):
    for j in range(img.height):
        p_rgb = px[i, j]
        print(p_rgb)
        p_hsv = colorsys.rgb_to_hsv(p_rgb[0] / 255, p_rgb[1] / 255, p_rgb[2] / 255)
        dh = (p_hsv[0] - base_hsv[0]) * 0.5 + 0.5
        ds = (p_hsv[1] - base_hsv[1]) * 0.5 + 0.5
        dv = (p_hsv[2] - base_hsv[2]) * 0.5 + 0.44
        if dh > 1: dh = 1
        if dh < 0: dh = 0
        if ds > 1: ds = 1
        if ds < 0: ds = 0
        if dv > 1: dv = 1
        if dv < 0: dv = 0
        new_rgb = (int(dh * 255), int(ds * 255), int(dv * 255), p_rgb[3])
        px[i, j] = new_rgb
img.save(sys.argv[1])