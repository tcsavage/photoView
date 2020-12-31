# Photo View

View Raw photographs and apply 3D .cube LUTs.

## Building the Flatpak

Install the KDE platform and SDK:
```
flatpak install flathub org.kde.Platform//5.15 org.kde.Sdk//5.15
```

Build and install locally:
```
flatpak-builder --user --install flatpak-build packaging/flatpak/dev.tomsavage.PhotoView.yml
```
