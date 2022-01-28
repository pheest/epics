#!/bin/sh
#
# Make tar for git repo w/ one level of sub modules.
#
set -e

die() {
  echo "$1" >&2
  exit 1
}

TOPREV="$1"
FINALTAR="$2"
PREFIX="$3"

if ! [ "$TOPREV" ]
then
   cat <<EOF >&2
usage: $0 <rev> [<outfile> [<prefix>]]

  <rev> may be any git revision spec. (tag, branch, or commit id).

  If provided, <outfile> must end with ".tar", ".tar.gz" or ".tar.bz2".
  If <outfile> is omitted, "base-<rev>.tar.gz" will be used.
  If provided, <prefix> must end with "/".  If <prefix> is omitted,
  the default is "base-<rev>/".
EOF
   exit 1
fi

case "$FINALTAR" in
"")
  TAROPT=-z
  FINALTAR="base-$TOPREV.tar.gz"
  ;;
*.tar)
  TAROPT=""
  ;;
*.tar.gz)
  TAROPT=-z
  ;;
*.tar.bz2)
  TAROPT=-j
  ;;
*)
  die "outfile must end with '.tar.gz' or '.tar.bz2'"
  ;;
esac

case "$PREFIX" in
"")
  PREFIX="base-$TOPREV/"
  ;;
*/)
  ;;
*)
  die "Prefix must end with '/'"
  ;;
esac

# Check for both <tag> and R<tag>
if [ "$TOPREV" = "HEAD" ]
then
  true
elif ! [ `git tag -l $TOPREV` ]
then
  if [ `git tag -l R$TOPREV` ]
  then
    TOPREV="R$TOPREV"
  else
    die "No tags exist '$TOPREV' or 'R$TOPREV'"
  fi
fi

# temporary directory w/ automatic cleanup
TDIR=`mktemp -d`
trap 'rm -rf "$TDIR"' EXIT INT QUIT TERM

mkdir "$TDIR"/tar

echo "Exporting revision $TOPREV as $FINALTAR with prefix $PREFIX"

# Use git-archive to copy files at the given revision into our temp. dir.
# Respects 'export-exclude' in .gitattributes files.

git archive --prefix=$PREFIX $TOPREV | tar -C "$TDIR"/tar -x

# use ls-tree instead of submodule-foreach to capture submodule revision associated with supermodule rev.
#
# sub-modules appear in tree as eg.:
#  160000 commit c3a6cfcf0dad4a4eeecf59b474710d06ff3eb68a  modules/ca
git ls-tree -r $TOPREV | awk '/^[0-9]+ commit / {print $3, $4}' | \
while read HASH MODDIR
do
    echo "Visiting $HASH $MODDIR"
    git -C $MODDIR archive --prefix=${PREFIX}${MODDIR}/ $HASH | tar -C "$TDIR"/tar -x
done

# make a list of files copied and filter out undesirables

(cd "$TDIR"/tar && find . -mindepth 1 -not -type d) > "$TDIR"/list.1

# Remove leading ./ from filenames
sed -i -e 's|^\./||' "$TDIR"/list.1

# Exclude files
sed \
  -e '/\/\.\?ci\//d' \
  -e '/\/\.tools\//d' \
  -e '/\/jenkins\//d' \
  -e '/\/\.git/d' \
  -e '/\/\.project$/d' \
  -e '/\/\.travis\.yml$/d' \
  -e '/\/\.appveyor\.yml$/d' \
  "$TDIR"/list.1 > "$TDIR"/list.2

if ! diff -U 0 "$TDIR"/list.1 "$TDIR"/list.2
then
    echo "Excluding the files shown above"
fi

# Use the filtered list to build the final tar
tar -c $TAROPT -C "$TDIR"/tar -T "$TDIR"/list.2 -f "$FINALTAR"

echo "Wrote $FINALTAR"

tar -t $TAROPT -f "$FINALTAR" > "$TDIR"/list.3

# make sure we haven't picked up anything extra
if ! diff -u "$TDIR"/list.2 "$TDIR"/list.3
then
    echo "Oops! Tarfile diff against plan shown above"
fi
