import { useState, useCallback, useEffect, useRef } from 'react'
import { Play, Loader2, AlertCircle, CheckCircle2, Copy, BookOpen, Sparkles, ExternalLink, Github, Download, Shuffle } from 'lucide-react'

// WASM module — lazy-loaded once
import initWasm, { validate_formula } from './wasm/west_rust.js'

function App() {
  const [activeTab, setActiveTab] = useState('tool')
  const [formula, setFormula] = useState('')
  const [loading, setLoading] = useState(false)
  const [wasmReady, setWasmReady] = useState(false)
  const [wasmError, setWasmError] = useState(null)
  const [result, setResult] = useState(null)
  const [error, setError] = useState(null)
  const [copied, setCopied] = useState(false)
  const [selectedSubf, setSelectedSubf] = useState(0)
  const wasmInitialized = useRef(false)

  // Initialize WASM module once on mount
  useEffect(() => {
    if (wasmInitialized.current) return
    wasmInitialized.current = true

    initWasm()
      .then(() => setWasmReady(true))
      .catch((err) => {
        console.error('WASM init failed:', err)
        setWasmError('Failed to load WASM module. Please refresh the page.')
      })
  }, [])

  const compileFormula = useCallback(() => {
    if (!formula.trim()) {
      setError('Please enter a formula')
      return
    }
    if (!wasmReady) {
      setError('WASM module is still loading. Please wait a moment.')
      return
    }

    setLoading(true)
    setError(null)
    setResult(null)
    setSelectedSubf(0)

    // Run in a microtask so the UI updates before the (synchronous) WASM call
    setTimeout(() => {
      try {
        const jsonStr = validate_formula(formula.trim())
        const data = JSON.parse(jsonStr)

        if (data.success) {
          // Also run the negation to get UNSAT patterns
          let negation_computations = []
          try {
            const negJsonStr = validate_formula(`!(${formula.trim()})`)
            const negData = JSON.parse(negJsonStr)
            if (negData.success) negation_computations = negData.computations
          } catch (_) { /* ignore — UNSAT button just stays disabled */ }

          // Map WASM result fields to what the UI expects
          const mapped = {
            success: true,
            nnf: data.nnf,
            computation_length: data.complen,
            bits_needed: data.bits_needed,
            prop_vars: data.variable_mapping.map((v) => v.name),
            total_computations: data.count,
            sample_computations: data.computations.slice(0, 50),
            subformulas: data.subformulas,
            negation_computations,
            raw_output: formatRawOutput(data),
          }
          setResult(mapped)
        } else {
          setError('Compilation failed')
        }
      } catch (err) {
        const msg = err instanceof Error ? err.message : String(err)
        setError(msg)
      } finally {
        setLoading(false)
      }
    }, 0)
  }, [formula, wasmReady])

  /** Build a human-readable raw output string from the WASM result */
  function formatRawOutput(data) {
    const lines = []
    lines.push(`Formula (NNF): ${data.nnf}`)
    lines.push(`Variables: ${data.variables}`)
    lines.push(`Variable Mapping: ${data.variable_mapping.map((v) => `${v.name} → ${v.index}`).join(', ')}`)
    lines.push(`Computation Length: ${data.complen}`)
    lines.push(`Bits Needed: ${data.bits_needed}`)
    lines.push(`Total Satisfying Computations: ${data.count}`)
    lines.push('')
    if (data.subformulas && data.subformulas.length > 0) {
      lines.push('--- Subformulas ---')
      for (const sf of data.subformulas) {
        lines.push(`  ${sf.formula}: ${sf.count} computation(s)`)
      }
      lines.push('')
    }
    lines.push('--- Computations ---')
    for (const c of data.computations) {
      lines.push(`  ${c}`)
    }
    return lines.join('\n')
  }

  const handleKeyDown = (e) => {
    if (e.key === 'Enter' && (e.metaKey || e.ctrlKey)) {
      compileFormula()
    }
  }

  const copyToClipboard = async (text) => {
    await navigator.clipboard.writeText(text)
    setCopied(true)
    setTimeout(() => setCopied(false), 2000)
  }

  const exampleFormulas = [
    { label: 'Simple Future', formula: 'F[0,5](p0)' },
    { label: 'Global', formula: 'G[0,10](p0)' },
    { label: 'Until', formula: '(p0) U[0,5] (p1)' },
    { label: 'Nested', formula: 'G[0,5](F[0,3](p0 & p1))' },
  ]

  return (
    <div className="min-h-screen bg-gradient-to-br from-slate-50 via-white to-west-50">
      {/* WASM Loading Overlay */}
      {wasmError && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-white/90">
          <div className="text-center p-8">
            <AlertCircle className="w-12 h-12 text-red-500 mx-auto mb-4" />
            <p className="text-red-700">{wasmError}</p>
          </div>
        </div>
      )}

      {/* Header */}
      <header className="border-b border-slate-200 bg-white/80 backdrop-blur-sm sticky top-0 z-10">
        <div className="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-3">
              <img src={`${import.meta.env.BASE_URL}west_logo.png`} alt="WEST logo" className="w-10 h-10 object-contain" />
              <div>
                <h1 className="text-xl font-bold text-slate-900">WEST</h1>
                <p className="text-xs text-slate-500">MLTL Formula Validation</p>
              </div>
            </div>
            <div className="flex items-center gap-6">
              <nav className="flex gap-1">
                <button
                  onClick={() => setActiveTab('tool')}
                  className={`px-3 py-1.5 rounded-lg text-sm font-medium transition-colors ${
                    activeTab === 'tool'
                      ? 'bg-west-100 text-west-700'
                      : 'text-slate-500 hover:text-slate-700 hover:bg-slate-100'
                  }`}
                >
                  Tool
                </button>
                <button
                  onClick={() => setActiveTab('about')}
                  className={`px-3 py-1.5 rounded-lg text-sm font-medium transition-colors ${
                    activeTab === 'about'
                      ? 'bg-west-100 text-west-700'
                      : 'text-slate-500 hover:text-slate-700 hover:bg-slate-100'
                  }`}
                >
                  About WEST
                </button>
              </nav>
              <a
                href="https://github.com/zwang271/WEST"
                target="_blank"
                rel="noopener noreferrer"
                className="text-slate-500 hover:text-slate-700 transition-colors"
              >
                <Github className="w-5 h-5" />
              </a>
            </div>
          </div>
        </div>
      </header>

      <main className="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8 py-8">

      {/* ===== ABOUT TAB ===== */}
      {activeTab === 'about' && (
        <div className="space-y-12">
          {/* About Section */}
          <section className="grid md:grid-cols-5 gap-8 items-start">
            <div className="md:col-span-3 space-y-4">
              <h2 className="text-2xl font-bold text-slate-900">About WEST</h2>
              <p className="text-slate-600 leading-relaxed">
                The WEST tool provides an automated way to generate regular expressions describing the set of all satisfying traces to Mission-time Linear Temporal Logic (MLTL) formulas. The graphic interface allows users to analyze MLTL formulas by randomly generating satisfying and unsatisfying traces, see how changing the truth value of variables at different steps affects the formula, import and export traces from files, and more.
              </p>
              <p className="text-slate-600 leading-relaxed">
                Please see our{' '}
                <a href="https://github.com/zwang271/WEST" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  GitHub
                </a>{' '}
                to install the WEST tool. We have also formally verified the algorithms in Isabelle/HOL, and our{' '}
                <a href="https://www.isa-afp.org/entries/Mission_Time_LTL_to_Regular_Expression.html" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  formalization
                </a>{' '}
                can be found on the Archive of Formal Proofs. We also have a software{' '}
                <a href="https://zenodo.org/records/14649154" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  artifact
                </a>{' '}
                available on Zenodo.
              </p>
            </div>
            <div className="md:col-span-2 flex flex-col gap-3">
              <a
                href="https://github.com/zwang271/WEST"
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-center justify-center gap-2 w-full px-6 py-3 rounded-full bg-slate-900 text-white font-medium hover:bg-slate-800 transition-colors"
              >
                <Github className="w-5 h-5" />
                GitHub
                <ExternalLink className="w-4 h-4 ml-1" />
              </a>
              <a
                href="https://www.isa-afp.org/entries/Mission_Time_LTL_to_Regular_Expression.html"
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-center justify-center gap-2 w-full px-6 py-3 rounded-full bg-west-600 text-white font-medium hover:bg-west-700 transition-colors"
              >
                Formalization
                <ExternalLink className="w-4 h-4 ml-1" />
              </a>
              <a
                href="https://zenodo.org/records/14649154"
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-center justify-center gap-2 w-full px-6 py-3 rounded-full bg-west-600 text-white font-medium hover:bg-west-700 transition-colors"
              >
                Paper Artifact
                <ExternalLink className="w-4 h-4 ml-1" />
              </a>
            </div>
          </section>

          {/* Publications */}
          <section>
            <h2 className="text-2xl font-bold text-slate-900 mb-6">Publications</h2>
            <ol className="space-y-4 list-decimal list-outside ml-6">
              <li className="text-slate-600 leading-relaxed pl-2">
                Jenna Elwing, Laura Gamboa Guzman, Jeremy Sorkins, Chiara Travesset, Zili Wang, Kristin Rozier.{' '}
                <em>Mission-time LTL (MLTL) Formula Validation Via Regular Expressions</em>, International Conference on integrated Formal Methods (iFM), 2023 Proceedings, available{' '}
                <a href="https://link.springer.com/chapter/10.1007/978-3-031-47705-8_15" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  here
                </a>.
              </li>
              <li className="text-slate-600 leading-relaxed pl-2">
                Zili Wang, Laura P. Gamboa Guzman, Kristin Y. Rozier.{' '}
                <em>WEST: Interactive Validation of Mission-time Linear Temporal Logic (MLTL)</em>, to appear in Science of Computer Programming, 2025, available{' '}
                <a href="https://research.temporallogic.org/papers/WGR25.pdf" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  here
                </a>.
              </li>
              <li className="text-slate-600 leading-relaxed pl-2">
                Zili Wang, Katherine Kosaian, Kristin Rozier.{' '}
                <em>Formally Verifying a Transformation from MLTL Formulas to Regular Expressions</em>, International Conference on Tools and Algorithms for the Construction and Analysis of Systems (TACAS), 2025, available{' '}
                <a href="https://link.springer.com/chapter/10.1007/978-3-031-90643-5_13" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  here
                </a>.
              </li>
            </ol>
          </section>

          {/* Contact */}
          <section className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 p-8">
            <p className="text-slate-600 text-center">
              Please contact{' '}
              <a href="https://zwang271.github.io/" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                Zili Wang
              </a>{' '}
              or the team at the{' '}
              <a href="https://laboratory.temporallogic.org/" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                Laboratory for Temporal Logic
              </a>{' '}
              for any questions regarding WEST.
            </p>
          </section>
        </div>
      )}

      {/* ===== TOOL TAB ===== */}
      {activeTab === 'tool' && (
        <>
        {/* Formula Input Section */}
        <section className="mb-8">
          <div className="bg-white rounded-2xl shadow-xl shadow-slate-200/50 border border-slate-200 overflow-hidden">
            <div className="flex min-h-0 divide-x divide-slate-100">
              {/* Left: formula input (70%) */}
              <div className="flex-[7] p-6 min-w-0">
                <label htmlFor="formula" className="block text-sm font-medium text-slate-700 mb-2">
                  MLTL Formula
                </label>
                <div className="relative">
                  <input
                    id="formula"
                    type="text"
                    value={formula}
                    onChange={(e) => setFormula(e.target.value)}
                    onKeyDown={handleKeyDown}
                    placeholder="Enter formula, e.g., G[0,10](p0 -> F[0,5](p1))"
                    className={`
                      formula-input w-full px-4 py-3 pr-24 rounded-xl border-2 
                      transition-all duration-200 outline-none
                      ${loading 
                        ? 'border-west-400 loading-border bg-west-50/50' 
                        : 'border-slate-200 hover:border-slate-300 focus:border-west-500 focus:ring-4 focus:ring-west-500/10'
                      }
                    `}
                    disabled={loading}
                  />
                  <button
                    onClick={compileFormula}
                    disabled={loading || !formula.trim() || !wasmReady}
                    className={`
                      absolute right-2 top-1/2 -translate-y-1/2
                      px-4 py-2 rounded-lg font-medium text-sm
                      flex items-center gap-2 transition-all duration-200
                      ${loading || !wasmReady
                        ? 'bg-west-100 text-west-600 cursor-wait'
                        : formula.trim()
                          ? 'bg-west-600 text-white hover:bg-west-700 shadow-lg shadow-west-600/25 hover:shadow-xl hover:shadow-west-600/30'
                          : 'bg-slate-100 text-slate-400 cursor-not-allowed'
                      }
                    `}
                  >
                    {!wasmReady ? (
                      <>
                        <Loader2 className="w-4 h-4 animate-spin" />
                        <span>Loading</span>
                      </>
                    ) : loading ? (
                      <>
                        <Loader2 className="w-4 h-4 animate-spin" />
                        <span>Running</span>
                      </>
                    ) : (
                      <>
                        <Play className="w-4 h-4" />
                        <span>Run</span>
                      </>
                    )}
                  </button>
                </div>
                <p className="mt-2 text-xs text-slate-500">
                  Press <kbd className="px-1.5 py-0.5 rounded bg-slate-100 font-mono text-[10px]">Ctrl</kbd>+<kbd className="px-1.5 py-0.5 rounded bg-slate-100 font-mono text-[10px]">Enter</kbd> to run
                </p>

                {/* NNF — shown inline after run */}
                {result && result.nnf && (
                  <div className="mt-3 pt-3 border-t border-slate-100 flex items-start justify-between gap-3">
                    <div className="min-w-0">
                      <p className="text-xs font-medium text-slate-400 uppercase tracking-wider mb-1">Negation Normal Form</p>
                      <code className="font-mono text-sm text-west-700 break-all">{result.nnf}</code>
                    </div>
                    <button
                      onClick={() => copyToClipboard(result.nnf)}
                      className="shrink-0 text-slate-400 hover:text-slate-600 transition-colors mt-0.5"
                      title="Copy NNF"
                    >
                      {copied ? <CheckCircle2 className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                    </button>
                  </div>
                )}
              </div>

              {/* Right: example formulas (30%) */}
              <div className="flex-[3] p-5 bg-slate-50">
                <p className="text-xs font-semibold text-slate-400 uppercase tracking-widest mb-3">Examples</p>
                <div className="space-y-2">
                  {exampleFormulas.map((ex) => (
                    <button
                      key={ex.formula}
                      onClick={() => setFormula(ex.formula)}
                      className="w-full text-left px-3 py-2.5 rounded-xl bg-white border border-slate-200 hover:border-west-300 hover:bg-west-50 transition-colors group"
                    >
                      <p className="text-xs font-semibold text-slate-700 group-hover:text-west-700 transition-colors">{ex.label}</p>
                      <code className="text-[10px] font-mono text-slate-400 group-hover:text-west-500 break-all transition-colors">{ex.formula}</code>
                    </button>
                  ))}
                </div>
              </div>
            </div>
          </div>
        </section>

        {/* Error Display */}
        {error && (
          <section className="mb-8">
            <div className="bg-red-50 border border-red-200 rounded-xl p-4 flex items-start gap-3">
              <AlertCircle className="w-5 h-5 text-red-500 flex-shrink-0 mt-0.5" />
              <div>
                <h3 className="font-medium text-red-800">Error</h3>
                <p className="text-sm text-red-700 mt-1">{error}</p>
              </div>
            </div>
          </section>
        )}

        {/* Results Section */}
        {result && result.success && (
          <section className="space-y-6">

            {/* Subformula selector + shared toggle table */}
            {result.subformulas && result.subformulas.length > 0 && (
              <div className="bg-white rounded-2xl shadow-xl shadow-slate-200/50 border border-slate-200 overflow-hidden">

                {/* Subformula selector header */}
                <div className="px-5 pt-5 pb-0 border-b border-slate-100">
                  <p className="text-xs font-semibold text-slate-400 uppercase tracking-widest mb-3">Subformulas</p>
                  <div className="flex gap-2 overflow-x-auto pb-4" style={{ scrollbarWidth: 'thin' }}>
                    {result.subformulas.map((sf, i) => (
                      <button
                        key={i}
                        onClick={() => setSelectedSubf(i)}
                        className={`flex-shrink-0 px-4 py-2.5 rounded-xl border-2 text-left transition-all duration-150 ${
                          selectedSubf === i
                            ? 'border-west-500 bg-west-50 shadow-md shadow-west-100/60'
                            : 'border-slate-200 bg-white hover:border-west-300 hover:bg-west-50/50'
                        }`}
                      >
                        <div className={`font-mono text-xs font-semibold ${
                          selectedSubf === i ? 'text-west-800' : 'text-slate-600'
                        }`}>{sf.formula}</div>
                        <div className={`text-[10px] mt-0.5 ${
                          selectedSubf === i ? 'text-west-500' : 'text-slate-400'
                        }`}>{sf.count.toLocaleString()} patterns</div>
                      </button>
                    ))}
                  </div>
                </div>

                {/* Toggle table for selected subformula */}
                <TraceTable
                  sf={result.subformulas[selectedSubf] ?? result.subformulas[0]}
                  n={result.prop_vars.length}
                  t={result.computation_length}
                  varNames={result.prop_vars}
                  resultKey={result.nnf}
                  unsatComputations={result.negation_computations ?? []}
                />
              </div>
            )}

            {/* Download Raw Output */}
            {result.raw_output && (
              <div className="flex justify-end">
                <button
                  onClick={() => {
                    const blob = new Blob([result.raw_output], { type: 'text/plain' })
                    const url = URL.createObjectURL(blob)
                    const a = document.createElement('a')
                    a.href = url
                    a.download = 'west_output.txt'
                    a.click()
                    URL.revokeObjectURL(url)
                  }}
                  className="flex items-center gap-2 px-4 py-2 rounded-lg bg-slate-100 hover:bg-slate-200 text-slate-600 text-sm font-medium transition-colors"
                >
                  <Download className="w-4 h-4" />
                  Download Raw Output
                </button>
              </div>
            )}
          </section>
        )}        </>
      )}
      </main>

      {/* Footer */}
      <footer className="border-t border-slate-200 mt-16">
        <div className="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8 py-6">
          <p className="text-center text-sm text-slate-500">
            WEST - Visualization Engine for Mission-time Linear Temporal Logic
          </p>
        </div>
      </footer>
    </div>
  )
}

function matchesPattern(trace, pattern) {
  if (trace.length !== pattern.length) return false
  for (let i = 0; i < trace.length; i++) {
    if (pattern[i] !== 's' && pattern[i] !== trace[i]) return false
  }
  return true
}

/** Resolve a regex pattern into a concrete toggle grid, randomising 's' wildcards */
function patternToToggle(pattern, n, t) {
  const segments = pattern.split(',')
  const grid = Array.from({ length: n }, () => Array(t).fill(false))
  for (let time = 0; time < t && time < segments.length; time++) {
    const seg = segments[time]
    for (let v = 0; v < n && v < seg.length; v++) {
      const ch = seg[v]
      grid[v][time] = ch === 's' ? Math.random() < 0.5 : ch === '1'
    }
  }
  return grid
}

function TraceTable({ sf, n, t, varNames, resultKey, unsatComputations = [] }) {
  const [toggle, setToggle] = useState(() =>
    Array.from({ length: n }, () => Array(t).fill(false))
  )

  // Reset toggle whenever the formula changes or dimensions change
  useEffect(() => {
    setToggle(Array.from({ length: n }, () => Array(t).fill(false)))
  }, [resultKey, n, t])

  // Build trace string: each timestep is n bits, joined by commas
  const traceStr = Array.from({ length: t }, (_, time) =>
    Array.from({ length: n }, (_, v) => (toggle[v]?.[time] ? '1' : '0')).join('')
  ).join(',')

  const isSat = sf.computations.some((c) => matchesPattern(traceStr, c))

  function setCell(v, time, val) {
    setToggle((prev) => {
      const next = prev.map((row) => [...row])
      next[v][time] = val
      return next
    })
  }

  function reset() {
    setToggle(Array.from({ length: n }, () => Array(t).fill(false)))
  }

  function applyPattern(pattern) {
    setToggle(patternToToggle(pattern, n, t))
  }

  function randomSat() {
    const patterns = sf.computations
    if (patterns.length === 0) return
    applyPattern(patterns[Math.floor(Math.random() * patterns.length)])
  }

  function randomUnsat() {
    if (unsatComputations.length === 0) return
    applyPattern(unsatComputations[Math.floor(Math.random() * unsatComputations.length)])
  }

  if (n === 0 || t === 0) return (
    <p className="text-xs text-slate-400 italic px-4 py-2">No variables to display.</p>
  )

  return (
    <div className="p-5 space-y-5">
      {/* Action bar */}
      <div className="flex items-center gap-2 flex-wrap">
        <button
          onClick={reset}
          className="text-xs px-2.5 py-1.5 bg-slate-100 rounded-lg hover:bg-slate-200 text-slate-500 font-medium transition-colors"
        >
          Reset Trace
        </button>
        <div className="w-px h-4 bg-slate-200 shrink-0" />
        <button
          onClick={randomSat}
          className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-west-600 hover:bg-west-700 text-white text-xs font-semibold shadow-sm shadow-west-600/25 transition-colors"
        >
          <Shuffle className="w-3.5 h-3.5" />
          Random SAT
        </button>
        <button
          onClick={randomUnsat}
          disabled={unsatComputations.length === 0}
          title={unsatComputations.length === 0 ? 'No unsatisfying traces available' : 'Load a random trace that does NOT satisfy the formula'}
          className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-semibold transition-colors ${
            unsatComputations.length > 0
              ? 'bg-red-500 hover:bg-red-600 text-white shadow-sm shadow-red-500/25'
              : 'bg-slate-100 text-slate-400 cursor-not-allowed'
          }`}
        >
          <Shuffle className="w-3.5 h-3.5" />
          Random UNSAT
        </button>
      </div>

      {/* SAT/UNSAT indicator */}
      <div className={`px-4 py-2.5 rounded-xl text-sm font-bold border ${
        isSat
          ? 'bg-green-50 text-green-700 border-green-200'
          : 'bg-red-50 text-red-700 border-red-200'
      }`}>
        {isSat ? '✓ Satisfied by this trace' : '✗ Not satisfied by this trace'}
      </div>

      {/* Trace string — prominent display */}
      <div className="rounded-xl bg-slate-900 px-4 py-3">
        <p className="text-[10px] font-semibold text-slate-400 uppercase tracking-widest mb-1">Current Trace</p>
        <code className="text-base font-mono text-green-400 break-all leading-relaxed">{traceStr}</code>
      </div>

      {/* Interactive table */}
      <div>
        <p className="text-xs font-semibold text-slate-400 uppercase tracking-widest mb-2">Interactive Trace Editor</p>
        <div className="overflow-x-auto rounded-xl border border-slate-200">
          <table className="text-xs border-collapse w-full">
            <thead className="bg-slate-50">
              <tr>
                <th className="px-3 py-2 text-left text-slate-500 font-medium border-b border-slate-200">Variable</th>
                {Array.from({ length: t }, (_, time) => (
                  <th key={time} className="px-3 py-2 text-center text-slate-500 font-medium min-w-[3.5rem] border-b border-slate-200">
                    t={time}
                  </th>
                ))}
              </tr>
            </thead>
            <tbody>
              {Array.from({ length: n }, (_, v) => (
                <tr key={v} className="border-b border-slate-100 last:border-0 hover:bg-slate-50 transition-colors">
                  <td className="px-3 py-2 font-mono font-medium text-slate-700 whitespace-nowrap bg-white">
                    {varNames[v] ?? `p${v}`}
                  </td>
                  {Array.from({ length: t }, (_, time) => (
                    <td key={time} className="px-3 py-2 text-center">
                      <input
                        type="checkbox"
                        checked={toggle[v]?.[time] ?? false}
                        onChange={(e) => setCell(v, time, e.target.checked)}
                        className="w-4 h-4 cursor-pointer accent-west-600"
                      />
                    </td>
                  ))}
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>

      {/* Computed regex patterns */}
      <div>
        <div className="flex items-end justify-between mb-2">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-widest">
            Computed Regex Patterns ({sf.count.toLocaleString()})
          </p>
          <span className="flex items-center gap-1.5 text-[10px] text-slate-400 italic">
            <span className="inline-block w-2.5 h-2.5 rounded-sm bg-green-300 border border-green-500 shrink-0" />
            matches current trace
          </span>
        </div>
        <div className="max-h-52 overflow-y-auto rounded-xl border border-slate-200 divide-y divide-slate-100" style={{ scrollbarWidth: 'thin' }}>
          {sf.computations.map((c, i) => {
            const isMatch = matchesPattern(traceStr, c)
            return (
              <div
                key={i}
                className={`flex items-center gap-2 px-3 py-1.5 ${
                  isMatch
                    ? 'bg-green-50 border-l-[3px] border-l-green-400'
                    : 'bg-white hover:bg-slate-50 border-l-[3px] border-l-transparent'
                }`}
              >
                <div className="flex items-center gap-0.5 shrink-0">
                  <button
                    onClick={() => applyPattern(c)}
                    className={`flex items-center gap-1 px-1.5 py-0.5 rounded text-[10px] font-semibold transition-colors ${
                      isMatch
                        ? 'bg-green-100 text-green-700 hover:bg-green-200'
                        : 'bg-slate-100 text-slate-500 hover:bg-west-100 hover:text-west-700'
                    }`}
                  >
                    <Shuffle className="w-2.5 h-2.5" />
                    use
                  </button>
                  <span
                    title="Click 'use' to randomly resolve this pattern's wildcards and load it into the trace editor"
                    className="text-[11px] text-slate-300 hover:text-slate-500 cursor-help select-none px-0.5"
                  >?</span>
                </div>
                <code className={`flex-1 min-w-0 font-mono text-xs break-all ${
                  isMatch ? 'text-green-800 font-semibold' : 'text-slate-500'
                }`}>
                  {c}
                </code>
                {isMatch && (
                  <span className="ml-auto shrink-0 inline-flex items-center px-1.5 py-0.5 rounded-md bg-green-100 text-green-700 text-[10px] font-bold tracking-wide">
                    ✓ match
                  </span>
                )}
              </div>
            )
          })}
        </div>
      </div>
    </div>
  )
}

export default App
