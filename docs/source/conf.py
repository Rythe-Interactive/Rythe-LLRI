# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Rythe-LLRI'
copyright = '2019, Raphael Baier, Rythe-Interactive Team'
author = 'Raphael Baier'

# The full version, including alpha/beta/rc tags
release = 'v0.0.1 alpha'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "recommonmark",
    "exhale"
]

breathe_projects = {
    "Rythe-LLRI": "../xml/",
}

source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'markdown',
    '.md': 'markdown',
    '.MD': 'markdown',
}

exhale_args = {

    "containmentFolder": "./api",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "LLRI API",
    "doxygenStripFromPath": "..",
    "createTreeView": False,
	"fullToctreeMaxDepth": 1,
}

primary_domain = 'cpp'

highlight_language = 'cpp'

breathe_default_project = "Rythe-LLRI"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# we are using a local theme, to not confuse
# python we need to insert ./ into python path
import os,sys

here = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0,here)

# our custom theme is basically just nameko with different colours
# we basically just need to change the template path
html_theme = 'nameko'

# now we can import our custom theme
import rythetheme

html_theme_path = [rythetheme.get_html_theme_path()]
