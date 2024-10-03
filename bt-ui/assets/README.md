# Resources
**icon.png** is the main file rest of them are generated from it

**icon.ico** is generated from **icon.png**

It can be re-generated with the help of **imagemagick** tool
```
magick icon.png icon.ico
```

generate 32x32 scaled logo
```
magick icon.png -resize 32x32\> icon_32x32.png
```

**icon.cpp** contains the raw data as array of bytes

Use xxd tool to re-generate array bytes
```
xxd -i icon_32x32.png > icon.cpp
```

