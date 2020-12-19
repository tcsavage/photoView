{ pkgs ? import <nixpkgs> {} }:
pkgs.qt5.mkDerivation {
    pname = "photo_luts";
    version = "0.0.0";
    src = ./.;
    nativeBuildInputs = [
      pkgs.gcc
      pkgs.pkg-config
      pkgs.cmake
    ];
    buildInputs = [
      pkgs.openimageio2
      pkgs.qt5.qtbase
    ];
}
