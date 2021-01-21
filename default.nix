{ pkgs ? import <nixpkgs> {} }:
let
  openimageio2 = with pkgs ; import ./nix/openimageio2.nix { inherit pkgs; };
in pkgs.callPackage (
  { gcc10, pkg-config, cmake, openexr, opencl-headers, ocl-icd, qt5 }:
  qt5.mkDerivationWith pkgs.stdenvNoCC.mkDerivation {
    pname = "photoView";
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
      opencl-headers
      ocl-icd
      qt5.qtbase
    ];
  }
) {}
