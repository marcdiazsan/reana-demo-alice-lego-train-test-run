===========================================
 REANA example - ALICE LEGO train test run
===========================================

.. image:: https://img.shields.io/travis/reanahub/reana-demo-alice-lego-train-test-run.svg
   :target: https://travis-ci.org/reanahub/reana-demo-alice-lego-train-test-run

.. image:: https://badges.gitter.im/Join%20Chat.svg
   :target: https://gitter.im/reanahub/reana?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge

.. image:: https://img.shields.io/github/license/reanahub/reana-demo-alice-lego-train-test-run.svg
   :target: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-test-run/master/LICENSE

About
=====

This `REANA <http://www.reana.io/>`_ reproducible analysis example performs
ALICE LEGO train test run and validation. The procedure is used in ALICE
collaboration particle physics analyses. Please see `arXiv:1502.06381
<https://arxiv.org/abs/1502.06381>`_ for more detailed description of the ALICE
analysis train system.

Analysis structure
==================

Making a research data analysis reproducible means to provide "runnable recipes"
addressing (1) where the input datasets are, (2) what software was used to
analyse the data, (3) which computing environment was used to run the software,
and (4) which workflow steps were taken to run the analysis.

1. Input data
-------------

This example uses ALICE Pb-Pb collision data input files. We can take a sample
Pb-Pb ESD open data file that the ALICE collaboration released on the `CERN Open
Data <http://opendata.cern.ch/>`_ portal, for example the following sample taken
at 3.5 TeV from run number 139038 in RunH 2010: (beware, the file is 360MB
large)

.. code-block:: console

   $ mkdir -p __alice__data__2010__LHC10h_2__000139038
   $ cd __alice__data__2010__LHC10h_2__000139038
   $ wget http://opendata.cern.ch/record/1102/files/assets/alice/2010/LHC10h/000139038/ESD/0003/AliESDs.root
   $ cd ..

Note that ``data.txt`` file should contain the path to the downloaded sample
data file.

2. Analysis code
----------------

This example uses the `AliPhysics <https://github.com/alisw/AliPhysics>`_
analysis framework with the following source code files:

- `env.sh <env.sh>`_ - ALICE LEGO train system configuration
- `generate.C <generate.C>`_ - a macro to generate macros to run the ALICE LEGO train
- `generator_customization.C <generator_customization.C>`_ - generator customisations
- `globalvariables.C <globalvariables.C>`_ - global variables
- `handlers.C <handlers.C>`_ - data access handlers (ESD/AOD, collision/MC)
- `runTest.sh <runTest.sh>`_ - run LEGO train test and validation
- `MLTrainDefinition.cfg <MLTrainDefinition.cfg>`_ - train wagon definitions
- `plot.C <plot.C>`_ - plot sample histogram

The user provides notably the `MLTrainDefinition.cfg <MLTrainDefinition.cfg>`_
file which defines a set of train wagons that compose the analysis train run. In
this example, the following wagons are defined:

.. code-block:: console

   $ grep Begin MLTrainDefinition.cfg
   #Module.Begin        Centrality_CF_AP
   #Module.Begin        PIDresponse_CF_AP
   #Module.Begin        Run1NetPiBASE_CF_AP

The first wagons are usually related to centralised data selection tasks, while
the main user analysis is executed in the last ``Run1NetPiBASE_CF_AP`` wagon.

The ``runTest.sh`` script will take care of creating the train test run, running
it, and validating its outputs.

3. Compute environment
----------------------

This example uses `AliPhysics <https://github.com/alisw/AliPhysics>`_ analysis
framework. It has been containerised as `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ environment. You can fetch
some wanted AliPhysics version from Docker Hub:

.. code-block:: console

   $ docker pull reanahub/reana-env-aliphysics:vAN-20180614-1

We shall use the ``vAN-20180614-1`` version for the present example.

Note that if you would like to build a different AliPhysics version on your own,
you can follow `reana-env-aliphysics
<https://github.com/reanahub/reana-env-aliphysics>`_ procedures and set
``ALIPHYSICS_VERSION`` environment variable appropriately:

.. code-block:: console

   $ cd src/reana-env-aliphysics
   $ export ALIPHYSICS_VERSION=vAN-20180521-1
   $ make build

4. Analysis workflow
--------------------

The researcher typically uses a single test run command:

.. code-block:: console

   $ ./runTest.sh

which performs all the tasks related to the analysis train generation, running
and validation. Underneath, the following sequence of commands is called:

.. code-block:: shell

   # generate the LEGO train run and validation files:
   aliroot -b -q generate.C > generation.log

   # perform the LEGO train test run:
   source ./lego_train.sh > stdout 2> stderr

   # verify that the expected result files are well present:
   source ./lego_train_validation.sh > validation.log

The produced log files indicate whether the train test run was successful and
whether the output is validated.

The computational workflow is therefore essentialy sequential in nature. We can
use the REANA serial workflow engine and represent the analysis workflow as
follows:

.. code-block:: console

              START
               |
               |
               V
   +----------------------------------------+
   | (1) download ESD input data file       |
   |                                        |
   |    $ wget http://opendata.cern.ch/...  |
   +----------------------------------------+
               |
               | ALIESD.root
               V
   +----------------------------------------+       +-------------------------+
   | (2) generate LEGO train files          |       |       input code        |
   |                                        |  <----|   MLTrainDefinition.cfg |
   |    $ aliroot -b -q generate.C          |       |   env.sh handlers.C ... |
   +----------------------------------------+       +-------------------------+
               |
               | lego_train.sh
               | lego_train_validation.sh
               | ...
               V
   +----------------------------------------+
   | (3) perform LEGO train test run        |
   |                                        |
   |    $ source ./lego_train.sh            |
   +----------------------------------------+
               |
               | stdout
               | AnalysisResults.root
               | ...
               V
   +----------------------------------------+
   | (4) validate test run outputs          |
   |                                        |
   |    $ source ./lego_train_validation.sh |
   +----------------------------------------+
               |
               | validation.log
               | AnalysisResults.root
               V
   +----------------------------------------+
   | (5) plot sample histogram              |
   |                                        |
   |    $ root -b -q plot.C                 |
   +----------------------------------------+
               |
               | plot.pdf
               V
              STOP

We shall see below how this sequence of commands is represented for the REANA
serial workflow engine.

5. Output results
-----------------

The output of the ALICE LEGO analysis train test run and validation is available
in the ``stdout`` file. The success or failure is reported at the end:

.. code-block:: console

   $ tail -4 stdout
   * ----------------------------------------------------*
   * ----------------   Job Validated  ------------------*
   * ----------------------------------------------------*
   *******************************************************

The test run will also create `ROOT <https://root.cern.ch/>`_ output files that
usually contain histograms.

.. code-block:: console

   $ ls -l AnalysisResults.root EventStat_temp.root
   -rw-r--r-- 1 root root 393111 May 30 17:35 EventStat_temp.root
   -rw-r--r-- 1 root root  31187 May 30 17:35 AnalysisResults.root

The user typically uses the output files to produce final plots. For example,
running ``plot.C`` output macro on the ``AnalysisResults.root`` output file will
permit to visualise the centrality of accepted events:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-alice-lego-train-test-run/master/docs/plot.png
   :alt: plot.png
   :align: center

Low centralities mean that the the Pb particles hit each other a lot and many
nucleons collide. High centralities mean that the Pb particles barely interacted
and only very few nucelons did collide.

Running the example on REANA cloud
==================================

We start by creating a `reana.yaml <reana.yaml>`_ file describing the above
analysis structure with its inputs, code, runtime environment, computational
workflow steps and expected outputs:

.. code-block:: yaml

    version: 0.3.0
    inputs:
      files:
      - MLTrainDefinition.cfg
      - data.txt
      - env.sh
      - generate.C
      - generator_customization.C
      - globalvariables.C
      - handlers.C
      - plot.C
      - runTest.sh
      - fix-env.sh
    workflow:
      type: serial
      specification:
        steps:
          - environment: 'reanahub/reana-env-aliphysics:vAN-20180614-1'
            commands:
            - 'mkdir __alice__data__2010__LHC10h_2__000139038/'
            - 'wget http://opendata.cern.ch/record/1102/files/assets/alice/2010/LHC10h/000139038/ESD/0003/AliESDs.root'
            - 'mv AliESDs.root __alice__data__2010__LHC10h_2__000139038/'
            - 'source fix-env.sh && source env.sh && aliroot -b -q generate.C | tee generation.log 2> generation.err'
            - 'source fix-env.sh && source env.sh && export ALIEN_PROC_ID=12345678 && source ./lego_train.sh | tee stdout 2> stderr'
            - 'source fix-env.sh && source env.sh && source ./lego_train_validation.sh | tee validation.log 2> validation.err'
            - 'source fix-env.sh && source env.sh && root -b -q ./plot.C'
    outputs:
      files:
      - plot.pdf


We can now install the REANA command-line client, run the analysis and download
the resulting plots:

.. code-block:: console

    $ # install REANA client
    $ mkvirtualenv reana-client
    $ pip install reana-client
    $ # connect to some REANA cloud instance
    $ export REANA_SERVER_URL=https://reana.cern.ch/
    $ export REANA_ACCESS_TOKEN=XXXXXXX
    $ # create new workflow
    $ reana-client create -n my-analysis
    $ export REANA_WORKON=my-analysis
    $ # upload input code and data to the workspace
    $ reana-client upload MLTrainDefinition.cfg data.txt \
      env.sh generate.C generator_customization.C globalvariables.C \
      handlers.C plot.C runTest.sh fix-env.sh
    $ # start computational workflow
    $ reana-client start
    $ # ... should be finished in about a minute
    $ reana-client status
    $ # list workspace files
    $ reana-client list
    $ # download output results
    $ reana-client download stdout
    $ reana-client download plot.pdf

Please see the `REANA-Client <https://reana-client.readthedocs.io/>`_
documentation for more detailed explanation of typical ``reana-client`` usage
scenarios.

Contributors
============

The list of contributors in alphabetical order:

- Markus Zimmermann
- `Rokas Maciulaitis <https://orcid.org/0000-0003-1064-6967>`_
- `Tibor Simko <https://orcid.org/0000-0001-7202-5803>`_
