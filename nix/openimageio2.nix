{ pkgs ? import <nixpkgs> {} }:

with pkgs;

stdenv.mkDerivation rec {
  pname = "openimageio";
  version = "2.2.8.0";

  src = fetchFromGitHub {
    owner = "OpenImageIO";
    repo = "oiio";
    rev = "Release-${version}";
    sha256 = "1ibf92pxpjmqqa83fjd3l61ccpb8nf8s0cin5nkvr1k89ggidp28";
  };

  outputs = [ "bin" "out" "dev" "doc" ];

  nativeBuildInputs = [
    cmake
    unzip
  ];

  buildInputs = [
    boost
    ilmbase
    libjpeg
    libpng
    libtiff
    libraw
    opencolorio
    openexr
    robin-map
    fmt
  ];

  cmakeFlags = [
    "-DUSE_PYTHON=OFF"
    "-DUSE_QT=OFF"
    # GNUInstallDirs
    # "-DCMAKE_INSTALL_LIBDIR=lib" # needs relative path for pkgconfig
  ];

  # GIT_SSL_CAINFO = "${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt";

  meta = with stdenv.lib; {
    homepage = "http://www.openimageio.org";
    description = "A library and tools for reading and writing images";
    license = licenses.bsd3;
    maintainers = with maintainers; [ goibhniu jtojnar ];
    platforms = platforms.unix;
  };
}
