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

import re
from pygments.lexers import CLexer
from pygments.token import Name, Keyword
from sphinx.highlighting import lexers

class LCLLexer(CLexer):
    name = "LCL"

    _luafunc = re.compile(r'(lua|moonL_)([a-zA-Z_]*)')
    _luaconst = re.compile(r'LUA_([a-zA-Z_]*)')
    _luatype = ['lua_State', 'luaL_Reg', 'luaC_Class']

    def get_tokens_unprocessed(self, text, stack=('root',)):
        for index, token, value in CLexer.get_tokens_unprocessed(self, text, stack):
            if token is Name and value in self._luatype:
                yield index, Keyword.Type, value
            elif token is Name and self._luafunc.search(value):
                yield index, Name.Function, value
            elif token is Name and self._luaconst.search(value):
                yield index, Keyword.Type, value #Keyword.Constant just looks like Name.Function
            else:
                yield index, token, value



lexers["LCL"] = LCLLexer()

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration
subprocess.call('doxygen Doxyfile.in', shell=True)

extensions = ['breathe', 'sphinxcontrib.luadomain']


templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

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
        "color-background-hover": "#eee8d5",
        "color-background-hover--transparent": "#fdf6e300",
        "color-background-border": "#93a1a1",
        "color-background-item": "#eee8d5",

        "color-brand-primary": "#268bd2",

        "color-api-background-hover": "#fdf6e3",
        "color-api-name": "#859900",
        "color-api-overall": "#cb4b16",
        "color-highlight-on-target": "#b58900ad",

        "color-highlighted-background": "#b58900ad",
        "color-highlighted-text": "#586e75",

        "color-admonition-background": "#fdf6e3"
        },
    "dark_css_variables": {
        "color-foreground-primary": "#839496",
        "color-foreground-secondary": "#93a1a1",
        "color-foreground-muted": "#586e75",
        "color-foreground-border": "#93a1a1",

        "color-background-primary": "#073642",
        "color-background-secondary": "#002b36",
        "color-background-hover": "#073642",
        "color-background-hover--transparent": "#002b3600",
        "color-background-border": "#586e75",
        "color-background-item": "#073642",

        "color-brand-primary": "#268bd2",

        "color-api-background-hover": "#002b36",
        "color-api-name": "#859900",
        "color-api-overall": "#cb4b16",
        "color-highlight-on-target": "#b58900ad",

        "color-highlighted-background": "#b58900ad",
        "color-highlighted-text": "#586e75",

        "color-admonition-background": "#002b36"
        },
    "footer_icons": [
        {
            "name": "GitHub",
            "url": "https://github.com/mousebyte/luaclasslib",
            "html": """
                <svg stroke="currentColor" fill="currentColor" stroke-width="0" viewBox="0 0 16 16">
                    <path fill-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68 0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.013 8.013 0 0 0 16 8c0-4.42-3.58-8-8-8z"></path>
                </svg>
            """,
            "class": "",
        }
        ]
}
pygments_style = "solarized-light"
pygments_dark_style = "solarized-dark"
#html_theme = 'alabaster'
html_static_path = ['_static']

default_role = "any"

breathe_projects = {
	"LuaClassLib": "_build/xml/"
}
breathe_default_project = "LuaClassLib"
breathe_default_members = ('members', 'undoc-members')
