{
  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      perSystem =
        {
          system,
          ...
        }:
        let
          pkgs = import inputs.nixpkgs {
            inherit system;
          };
          raylib-sdl = (pkgs.raylib.override { platform = "SDL"; });
        in
        {
          devShells.default = pkgs.mkShell {
            packages = with pkgs; [
              gcc
              ispc
              cmake
              valgrind
              clang-tools
              raylib-sdl
              perf 
              flamegraph
              gifsicle
            ];
            LD_LIBRARY_PATH ="${pkgs.ispc}/lib";
            NIX_ENFORCE_NO_NATIVE=0; 
          };
        };
    };
}
