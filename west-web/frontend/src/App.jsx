import { useState, useCallback, useEffect, useRef } from 'react'
import { Play, Loader2, AlertCircle, CheckCircle2, Copy, BookOpen, Sparkles, ExternalLink, Github } from 'lucide-react'

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

    // Run in a microtask so the UI updates before the (synchronous) WASM call
    setTimeout(() => {
      try {
        const jsonStr = validate_formula(formula.trim())
        const data = JSON.parse(jsonStr)

        if (data.success) {
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
            <div className="p-6">
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
            </div>

            {/* Example Formulas */}
            <div className="px-6 py-3 bg-slate-50 border-t border-slate-100">
              <div className="flex items-center gap-2 flex-wrap">
                <span className="text-xs text-slate-500">Examples:</span>
                {exampleFormulas.map((ex) => (
                  <button
                    key={ex.formula}
                    onClick={() => setFormula(ex.formula)}
                    className="px-2.5 py-1 rounded-md bg-white border border-slate-200 text-xs font-mono text-slate-600 hover:border-west-300 hover:text-west-700 hover:bg-west-50 transition-colors"
                  >
                    {ex.label}
                  </button>
                ))}
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
            {/* Summary Cards */}
            <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
              <ResultCard label="Computation Length" value={result.computation_length ?? '-'} />
              <ResultCard label="Bits Needed" value={result.bits_needed ?? '-'} />
              <ResultCard label="Prop Variables" value={result.prop_vars?.length ?? '-'} />
              <ResultCard label="Total Computations" value={result.total_computations ?? '-'} />
            </div>

            {/* NNF */}
            {result.nnf && (
              <div className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 overflow-hidden">
                <div className="px-4 py-3 bg-slate-50 border-b border-slate-200 flex items-center justify-between">
                  <h3 className="font-medium text-slate-700">Negation Normal Form (NNF)</h3>
                  <button
                    onClick={() => copyToClipboard(result.nnf)}
                    className="text-slate-400 hover:text-slate-600 transition-colors"
                  >
                    {copied ? <CheckCircle2 className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                  </button>
                </div>
                <div className="p-4">
                  <code className="font-mono text-sm text-west-700 break-all">{result.nnf}</code>
                </div>
              </div>
            )}

            {/* Propositional Variables */}
            {result.prop_vars && result.prop_vars.length > 0 && (
              <div className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 overflow-hidden">
                <div className="px-4 py-3 bg-slate-50 border-b border-slate-200">
                  <h3 className="font-medium text-slate-700">Propositional Variables</h3>
                </div>
                <div className="p-4 flex flex-wrap gap-2">
                  {result.prop_vars.map((v, i) => (
                    <span key={i} className="px-3 py-1 rounded-full bg-west-100 text-west-700 font-mono text-sm">
                      {v}
                    </span>
                  ))}
                </div>
              </div>
            )}

            {/* Sample Computations */}
            {result.sample_computations && result.sample_computations.length > 0 && (
              <div className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 overflow-hidden">
                <div className="px-4 py-3 bg-slate-50 border-b border-slate-200">
                  <h3 className="font-medium text-slate-700">
                    Satisfying Computations
                    <span className="ml-2 text-sm font-normal text-slate-500">
                      (showing {result.sample_computations.length} of {result.total_computations})
                    </span>
                  </h3>
                </div>
                <div className="p-4 overflow-x-auto">
                  <div className="space-y-2">
                    {result.sample_computations.map((comp, i) => (
                      <div key={i} className="font-mono text-sm text-slate-600 bg-slate-50 px-3 py-2 rounded-lg">
                        <span className="text-slate-400 mr-3">{(i + 1).toString().padStart(2, '0')}</span>
                        {comp}
                      </div>
                    ))}
                  </div>
                </div>
              </div>
            )}

            {/* Subformulas (collapsible) */}
            {result.subformulas && result.subformulas.length > 1 && (
              <details className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 overflow-hidden">
                <summary className="px-4 py-3 bg-slate-50 border-b border-slate-200 cursor-pointer hover:bg-slate-100 transition-colors">
                  <span className="font-medium text-slate-700">
                    Subformula Analysis
                    <span className="ml-2 text-sm font-normal text-slate-500">
                      ({result.subformulas.length} subformulas)
                    </span>
                  </span>
                </summary>
                <div className="p-4 space-y-4">
                  {result.subformulas.map((sf, i) => (
                    <div key={i} className="border border-slate-100 rounded-lg overflow-hidden">
                      <div className="px-3 py-2 bg-slate-50 flex items-center justify-between">
                        <code className="font-mono text-sm text-west-700">{sf.formula}</code>
                        <span className="text-xs text-slate-500">{sf.count} computation(s)</span>
                      </div>
                      {sf.computations.length > 0 && sf.computations.length <= 20 && (
                        <div className="px-3 py-2 space-y-1">
                          {sf.computations.map((c, j) => (
                            <div key={j} className="font-mono text-xs text-slate-500">{c}</div>
                          ))}
                        </div>
                      )}
                    </div>
                  ))}
                </div>
              </details>
            )}

            {/* Raw Output (collapsible) */}
            <details className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 overflow-hidden">
              <summary className="px-4 py-3 bg-slate-50 border-b border-slate-200 cursor-pointer hover:bg-slate-100 transition-colors">
                <span className="font-medium text-slate-700">Raw Output</span>
              </summary>
              <div className="p-4">
                <pre className="code-block whitespace-pre-wrap">{result.raw_output}</pre>
              </div>
            </details>
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

function ResultCard({ label, value }) {
  return (
    <div className="bg-white rounded-xl shadow-lg shadow-slate-200/50 border border-slate-200 p-4">
      <p className="text-xs text-slate-500 uppercase tracking-wider mb-1">{label}</p>
      <p className="text-2xl font-bold text-slate-900">{value}</p>
    </div>
  )
}

export default App
