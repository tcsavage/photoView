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
      pkgs.opencl-headers
      pkgs.ocl-icd
      pkgs.qt5.qtbase
      pkgs.flatpak-builder
      pkgs.linuxPackages.perf
  ];
  shellHook = ''
    export QT_PLUGIN_PATH=${pkgs.qt5.qtbase.bin}/${pkgs.qt5.qtbase.qtPluginPrefix}
    alias clean="rm -rf build"
    alias config="cmake -Bbuild -S. -DCMAKE_BUILD_TYPE=RelWithDebInfo"
    alias configDebug="cmake -Bbuild -S. -DCMAKE_BUILD_TYPE=Debug"
    alias build="cmake --build build"
  '';
}
