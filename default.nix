{ pkgs ? import <nixpkgs> {} }:
let
  openimageio2 = with pkgs ; import ./nix/openimageio2.nix { inherit pkgs; };
in pkgs.callPackage (
  { gcc10, pkg-config, cmake, openexr, qt5 }:
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
