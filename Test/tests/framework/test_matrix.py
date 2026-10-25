"""The configuration matrix of the library: the fragments harness.host.matrix writes, the rows of
config/matrix.toml. The run itself is the nox session `matrix`; these are the checks of its data.

Proofs (ALX-1564):
  P369 every fragment under config/matrix/ is the one harness.host.matrix writes from
       alxConfig_TEMPLATE.h, and nothing else is there
  P370 every row of config/matrix.toml names words that have a fragment and a meaning here - a
       platform table for its platform, gcc or clang for its toolchain, clang alone on the host -
       and a new row takes the next id at the end
  P371 every switch of the template's Platform, Features, Target and Library categories is set or
       cleared by some fragment, the library guard ALX_C_LIB alone excepted, and every name a
       fragment sets is declared by the template
"""

from alx.verify import configs

from harness.host import matrix as hm


def test_ALX1564_P369_the_fragments_are_the_ones_the_harness_writes_from_the_template():
    on_disk = {path.relative_to(hm.FRAGMENTS).as_posix(): path.read_text(encoding="ascii")
               for path in sorted(hm.FRAGMENTS.rglob("*.h"))}
    assert on_disk == hm.fragments(), "run `python -m harness.host.matrix` from Test/"


def test_ALX1564_P370_every_row_names_words_with_a_fragment_and_a_meaning():
    m = configs.load_matrix(hm.MATRIX_FILE, hm.TEST)
    assert m.rows
    assert configs.missing_fragments(m.rows, hm.FRAGMENTS) == []
    for row in m.rows:
        platform = m.platforms[row.words["platform"]]
        assert row.words["toolchain"] in ({"clang"} if platform.host else {"gcc", "clang"}), row.name
    ids = [int(row.id) for row in m.rows]
    assert ids == sorted(set(ids)), "a new row takes the next free id, at the end"


def test_ALX1564_P371_every_switch_that_selects_code_is_varied_by_some_fragment():
    groups = hm.template_groups()
    declared = {name for names in groups.values() for name in names}
    varied = {name for (category, _), names in groups.items()
              if category in ("Platform", "Features", "Target", "Library") for name in names}
    touched = {name for path in hm.FRAGMENTS.rglob("*.h") for _, name, _ in configs.parse_fragment(path)}
    assert varied - touched == {"ALX_C_LIB"}
    assert touched - declared == set()
