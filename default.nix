{ pkgs ? import <nixpkgs> {} }:
pkgs.callPackage (
  { gcc10, pkg-config, cmake, openimageio2, openexr, qt5 }:
  qt5.mkDerivation {
    pname = "photo_luts";
    version = "0.0.0";
    src = ./.;
    nativeBuildInputs = [
      gcc10
      pkg-config
      cmake
    ];
    buildInputs = [
      openimageio2
      openexr
      qt5.qtbase
    ];
  }
) {}
