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
pygments_style = "sphinx"
pygments_dark_style = "monokai"
#html_theme = 'alabaster'
html_static_path = ['_static']

breathe_projects = {
	"LuaClassLib": "_build/xml/"
}
breathe_default_project = "LuaClassLib"
breathe_default_members = ('members', 'undoc-members')
