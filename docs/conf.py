# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import subprocess

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'LuaClassLib'
copyright = '2023, Abigail Teague'
author = 'Abigail Teague'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration
subprocess.call('doxygen Doxyfile.in', shell=True)

extensions = ['breathe']


templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

highlight_language = 'c'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output
html_theme = 'furo'
html_theme_options = {
    "light_css_variables": {
        "color-foreground-primary": "#657b83",
        "color-foreground-secondary": "#586e75",
        "color-foreground-muted": "#93a1a1",
        "color-foreground-border": "#586e75",

        "color-background-primary": "#eee8d5",
        "color-background-secondary": "#fdf6e3",
        "color-background-hover": "#fdf6e3",
        "color-background-hover--transparent": "#fdf6e300",
        "color-background-border": "#93a1a1",
        "color-background-item": "#eee8d5",

        "color-brand-primary": "#268bd2",

        "color-api-background-hover": "#fdf6e3",
        "color-api-name": "#859900",
        "color-api-overall": "#cb4b16",
        "color-highlight-on-target": "#b58900ad",

        "color-highlighted-background": "#b58900ad",
        "color-highlighted-text": "#586e75"
        },
    "dark_css_variables": {
        "color-foreground-primary": "#839496",
        "color-foreground-secondary": "#93a1a1",
        "color-foreground-muted": "#586e75",
        "color-foreground-border": "#93a1a1",

        "color-background-primary": "#073642",
        "color-background-secondary": "#002b36",
        "color-background-hover": "#002b36",
        "color-background-hover--transparent": "#002b3600",
        "color-background-border": "#586e75",
        "color-background-item": "#073642",

        "color-brand-primary": "#268bd2",

        "color-api-background-hover": "#002b36",
        "color-api-name": "#859900",
        "color-api-overall": "#cb4b16",
        "color-highlight-on-target": "#b58900ad",

        "color-highlighted-background": "#b58900ad",
        "color-highlighted-text": "#586e75"
        } 
}
pygments_style = "solarized-light"
pygments_dark_style = "solarized-dark"
#html_theme = 'alabaster'
html_static_path = ['_static']

breathe_projects = {
	"LuaClassLib": "_build/xml/"
}
breathe_default_project = "LuaClassLib"
breathe_default_members = ('members', 'undoc-members')
