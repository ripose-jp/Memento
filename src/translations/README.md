# Translations

Translation source files (`*.ts`) live here and are updated by Qt Linguist.
Generated catalogs (`*.qm`) are build artifacts and should not be edited by hand.

Run the translation targets from a configured build directory:

```sh
cmake --build build --target update_translations
cmake --build build --target release_translations
```

`Memento` is the application name. Do not translate it in any language, including
inside longer strings such as `About Memento`.
