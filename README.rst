==================================================
 REANA demo example - ALICE LEGO train validation
==================================================

.. image:: https://img.shields.io/travis/reanahub/reana-demo-alice-lego-train-validation.svg
   :target: https://travis-ci.org/reanahub/reana-demo-alice-lego-train-validation

.. image:: https://badges.gitter.im/Join%20Chat.svg
   :target: https://gitter.im/reanahub/reana?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge

.. image:: https://img.shields.io/github/license/reanahub/reana-demo-alice-lego-train-validation.svg
   :target: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-validation/master/LICENSE

About
=====

This `REANA <http://reanahub.io/>`_ demo example runs ALICE LEGO train validation.

Analysis structure
==================

Making a research data analysis reproducible means to provide "runnable recipes"
addressing (1) where the input datasets are, (2) what software was used to
analyse the data, (3) which computing environment was used to run the software,
and (4) which workflow steps were taken to run the analysis.

1. Input dataset
---------------

This example uses ALICE proton-proton data sample input files. You will need to
take some Pb-Pb ESD file from `CERN Open Data portal
<http://opendata.cern.ch/>_. For example, the following sample taken at 3.5 TeV
from RunB in 2010: (beware, the file which is 231 MB big)

.. code-block:: console

   $ mkdir -p __alice__data__2011__LHC11h_2__000170387
   $ cd __alice__data__2011__LHC11h_2__000170387
   $ wget http://opendata.cern.ch/record/1100/files/assets/alice/2010/LHC10b/000117222/ESD/0001/AliESDs.root .
   $ cd ..

Please make sure that ``data.txt`` contains well the absolute path to the sample
data file.

2. Analysis code
----------------

This example uses the `AliPhysics <https://github.com/alisw/AliPhysics>`_
analysis framework with the custom user code located in the ``code`` directory:

- `generate.C <generate.C>`_ - a macro to generate macros for the ALICE LEGO train
- `globalvariables.C <globalvariables.C>`_ - global variables
- `handlers.C <handlers.C>`_ - handlers
- `runTest.sh <runTest.sh>`_ - run LEGO train validation

Note also that this example uses the following additional environment
configuration:

- `env.sh <env.sh>`_ - ALICE LEGO train system environment variables
- `MLTrainDefinition.cfg <MLTrainDefinition.cfg>`_ - train definition file

3. Compute environment
----------------------

This example uses `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ containerised environment.
You can see and use one of the provided AliPhysics containers:

.. code-block:: console

   $ docker pull reanahub/reana-env-aliphysics:vAN-20170521-1

or you can build your own by setting the ``ALIPHYSICS_VERSION`` environment
variable, for example:

.. code-block:: console

   $ cd src/reana-env-aliphysicsb
   $ export ALIPHYSICS_VERSION=vAN-20170521-1
   $ make build

Please see `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ documentation for more
information.

4. Analysis workflow
--------------------

This analysis example runs ALICE LEGO train validation and test run. We can use
a simple set of shell commands that will run one after another, so our workflow
is a very simple sequence:

.. code-block:: text

   $ # generate the train files saving output in a log file:
   $ aliroot -b -q generate.C > generation.log

   $ # run lego train and save outputs and errors:
   $ source ./lego_train.sh > stdout 2> stderr

   $ # verify that the expected result files are well present:
   $ source ./lego_train_validation.sh > validation.log

The analysis example will produce two output `ROOT <https://root.cern.ch/>`_
files:

.. code-block:: console

   $ ls -1 GammaConvFlow_69.root EventStat_temp.root
   EventStat_temp.root
   GammaConvFlow_69.root

Local testing with Docker
=========================

We can check whether our example works locally using `Docker
<https://www.docker.com/>`_ directly:

.. code-block:: console

   $ docker run -i -t --rm -v `pwd`:/inputs \
        reanahub/reana-env-aliphysics:vAN-20170521-1 \
        'cd /inputs && ./runTest.sh'

The example will run for about a minute and will report about validation success:

.. code-block:: console

   $ tail -4 stdout
   * ----------------------------------------------------*
   * ----------------   Job Validated  ------------------*
   * ----------------------------------------------------*
   *******************************************************

and produce the two expected ROOT files:

.. code-block:: console

   $ ls -l GammaConvFlow_69.root EventStat_temp.root
   -rw-r--r-- 1 root root 999737 May 30 17:35 EventStat_temp.root
   -rw-r--r-- 1 root root 273102 May 30 17:35 GammaConvFlow_69.root

We can now plot the histogram:

.. code-block:: console

   $ docker run -i -t --rm -v `pwd`:/inputs \
        reanahub/reana-env-aliphysics:vAN-20170521-1 \
        'cd /inputs && root -b -q ./outputMacro.C'

which produces a PDF output file:

   $ ls -l output.pdf
   -rw-r--r-- 1 root root 14238 May 30 17:37 output.pdf

that should look like this:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-validation/master/docs/output.png
   :alt: output.png
   :align: center

Running the example on REANA cloud
==================================

**FIXME**

Contributors
============

The list of contributors in alphabetical order:

- Markus Zimmermann <m.zimmermann@cern.ch>
- `Tibor Simko <https://orcid.org/0000-0001-7202-5803>`_ <tibor.simko@cern.ch>
