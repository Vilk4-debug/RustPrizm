#!/bin/sh -e

mkdir -p dist

find . -type f \( \
    -name "RustPrizm*.tar.gz" -o \
    -name "RustPrizm*.exe" -o \
    -name "RustPrizm*.zip" -o \
    -name "RustPrizm*.AppImage*" -o \
    -name "PolyMC*.tar.gz" -o \
    -name "PolyMC*.exe" -o \
    -name "PolyMC*.zip" -o \
    -name "PolyMC*.AppImage*" \
\) -not -path "./dist/*" -exec cp {} dist/ \;

# mk source tarball
if [ -d "RustPrizm-source" ]; then
    mv RustPrizm-source RustPrizm-"${VERSION}"
    tar czf "dist/RustPrizm-$VERSION.tar.gz" "RustPrizm-${VERSION}"
elif [ -d "PolyMC-source" ]; then
    mv PolyMC-source PolyMC-"${VERSION}"
    tar czf "dist/PolyMC-$VERSION.tar.gz" "PolyMC-${VERSION}"
fi

echo "-- artifacts installed in $PWD/dist"

ls -lh dist
