{ pkgs ? import <nixpkgs> {} }:
let
 openimageio2 = with pkgs ; import ./nix/openimageio2.nix { inherit pkgs; };
in pkgs.stdenvNoCC.mkDerivation {
  name = "photo-view-gcc-environment";
  buildInputs = [
      pkgs.gcc10
      pkgs.gdb
      pkgs.pkg-config
      pkgs.cmake
      openimageio2
      pkgs.libraw
      pkgs.openexr
      pkgs.qt5.qtbase
      pkgs.flatpak-builder
      pkgs.linuxPackages.perf
  ];
}
