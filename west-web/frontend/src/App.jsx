import { useState, useCallback, useEffect, useRef } from 'react'
import { Play, Loader2, AlertCircle, CheckCircle2, Copy, ExternalLink, Github, Download, Shuffle, ChevronDown, ChevronUp, ChevronLeft, ChevronRight, Menu, X, HelpCircle } from 'lucide-react'

// WASM module — lazy-loaded once
import initWasm, { validate_formula } from './wasm/west_rust.js'

function tabFromPath(path) {
  if (path === '/tool') return 'tool'
  if (path === '/syntax') return 'syntax'
  return 'about'
}

function App() {
  const [activeTab, setActiveTab] = useState(() => tabFromPath(window.location.pathname))
  const [formula, setFormula] = useState('')
  const [loading, setLoading] = useState(false)
  const [wasmReady, setWasmReady] = useState(false)
  const [wasmError, setWasmError] = useState(null)
  const [result, setResult] = useState(null)
  const [error, setError] = useState(null)
  const [copied, setCopied] = useState(false)
  const [selectedSubf, setSelectedSubf] = useState(0)
  const [examplesOpen, setExamplesOpen] = useState(true)
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [nnfOpen, setNnfOpen] = useState(false)
  const [subfListOpen, setSubfListOpen] = useState(true)
  const wasmInitialized = useRef(false)
  const subformulasRef = useRef(null)

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

  // Sync with browser back/forward
  useEffect(() => {
    const handlePop = () => setActiveTab(tabFromPath(window.location.pathname))
    window.addEventListener('popstate', handlePop)
    return () => window.removeEventListener('popstate', handlePop)
  }, [])

  function navigate(tab) {
    const path = tab === 'about' ? '/' : `/${tab}`
    history.pushState({}, '', path)
    setActiveTab(tab)
  }

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
    setNnfOpen(false)

    // Run in a microtask so the UI updates before the (synchronous) WASM call
    setTimeout(() => {
      try {
        const jsonStr = validate_formula(formula.trim())
        const data = JSON.parse(jsonStr)

        if (data.success) {
          // Map WASM result fields to what the UI expects
          // Each subformula now includes its own unsat_computations from the backend
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
          setTimeout(() => {
            subformulasRef.current?.scrollIntoView({ behavior: 'smooth', block: 'start' })
          }, 100)
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
    { label: 'The only two sure things in life...', formula: 'F[0,10](death and taxes)' },
    { label: 'Always stop when you see a red light', formula: 'G[0,5](red_light -> stop_car)' },
    { label: 'Tired until you sleep', formula: 'Tired U[0,5] Sleep' },
    { label: 'Alternating On and Off', formula: 'G[0,10]((on -> (F[1,1] !on)) && \n (!on -> (F[1,1] on)))' },
    { label: 'Every request gets a timely response', formula: 'G[0,5](request -> F[1,3](response))' },
    { label: 'Eventually goal is reached and no error', formula: 'F[0,5](goal & !error)' },
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

      {/* Sidebar overlay (mobile) */}
      {sidebarOpen && (
        <div className="fixed inset-0 bg-black/30 z-30" onClick={() => setSidebarOpen(false)} />
      )}

      {/* Sidebar */}
      <aside className={`fixed top-0 left-0 h-full z-40 bg-white border-r border-slate-200 flex flex-col transition-transform duration-200 w-56 ${
        sidebarOpen ? 'translate-x-0' : '-translate-x-full'
      }`}>
        <div className="flex items-center justify-between px-4 py-4 border-b border-slate-100">
          <div className="flex items-center gap-2">
            <img src={`${import.meta.env.BASE_URL}west_logo.png`} alt="WEST logo" className="w-8 h-8 object-contain" />
            <div>
              <h1 className="text-lg font-bold text-slate-900 leading-tight">WEST</h1>
              <p className="text-[10px] text-slate-600">MLTL Formula Validation</p>
            </div>
          </div>
          <button onClick={() => setSidebarOpen(false)} className="text-slate-500 hover:text-slate-800 transition-colors">
            <X className="w-5 h-5" />
          </button>
        </div>
        <nav className="flex-1 px-3 py-4 space-y-1">
          <button
            onClick={() => { navigate('about'); setSidebarOpen(false) }}
            className={`w-full text-left px-3 py-2 rounded-lg text-sm font-medium transition-colors ${
              activeTab === 'about'
                ? 'bg-west-100 text-west-700'
                : 'text-slate-700 hover:text-slate-900 hover:bg-slate-100'
            }`}
          >
            About WEST
          </button>
          <button
            onClick={() => { navigate('tool'); setSidebarOpen(false) }}
            className={`w-full text-left px-3 py-2 rounded-lg text-sm font-medium transition-colors ${
              activeTab === 'tool'
                ? 'bg-west-100 text-west-700'
                : 'text-slate-700 hover:text-slate-900 hover:bg-slate-100'
            }`}
          >
            Tool
          </button>
          <button
            onClick={() => { navigate('syntax'); setSidebarOpen(false) }}
            className={`w-full text-left px-3 py-2 rounded-lg text-sm font-medium transition-colors ${
              activeTab === 'syntax'
                ? 'bg-west-100 text-west-700'
                : 'text-slate-700 hover:text-slate-900 hover:bg-slate-100'
            }`}
          >
            MLTL Syntax
          </button>
        </nav>
        <div className="px-3 py-4 border-t border-slate-100">
          <a
            href="https://github.com/zwang271/WEST"
            target="_blank"
            rel="noopener noreferrer"
            className="flex items-center gap-2 px-3 py-2 rounded-lg text-sm text-slate-700 hover:text-slate-900 hover:bg-slate-100 transition-colors"
          >
            <Github className="w-4 h-4" />
            GitHub
            <ExternalLink className="w-3 h-3 ml-auto" />
          </a>
        </div>
      </aside>

      {/* Top bar: just logo + hamburger */}
      <header className="sticky top-0 z-20 bg-white/80 backdrop-blur-sm border-b border-slate-200">
        <div className="max-w-screen-2xl mx-auto px-4 sm:px-6 lg:px-10 py-3 flex items-center gap-3">
          <button onClick={() => setSidebarOpen(true)} className="text-slate-700 hover:text-slate-900 transition-colors">
            <Menu className="w-5 h-5" />
          </button>
          <button onClick={() => navigate('about')} className="flex items-center gap-2 hover:opacity-75 transition-opacity">
            <img src={`${import.meta.env.BASE_URL}west_logo.png`} alt="WEST logo" className="w-7 h-7 object-contain" />
            <h1 className="text-lg font-bold text-slate-900">WEST</h1>
          </button>
        </div>
      </header>

      <main className="max-w-screen-2xl mx-auto px-4 sm:px-6 lg:px-10 py-8">

      {/* ===== ABOUT TAB ===== */}
      {activeTab === 'about' && (
        <div className="space-y-12">
          {/* About Section */}
          <section className="grid md:grid-cols-5 gap-8 items-start">
            <div className="md:col-span-3 space-y-4">
              <h2 className="text-2xl font-bold text-slate-900">About WEST</h2>
              <p className="text-slate-800 leading-relaxed">
                The WEST tool provides an automated way to generate regular expressions describing the set of all satisfying traces to Mission-time Linear Temporal Logic (MLTL) formulas. The graphic interface allows users to analyze MLTL formulas by randomly generating satisfying and unsatisfying traces, see how changing the truth value of variables at different steps affects the formula, import and export traces from files, and more.
              </p>
              <p className="text-slate-800 leading-relaxed">
                Please see our{' '}
                <a href="https://github.com/zwang271/WEST" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  GitHub
                </a>{' '}
                to install the WEST tool. We have also formally verified the algorithms in Isabelle/HOL, and our{' '}
                <a href="https://www.isa-afp.org/entries/Mission_Time_LTL_to_Regular_Expression.html" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  formalization
                </a>{' '}
                can be found on the Archive of Formal Proofs.
              </p>
            </div>
            <div className="md:col-span-2 flex flex-col gap-3">
              <button
                onClick={() => navigate('tool')}
                className="flex items-center justify-center gap-2 w-full px-6 py-3 rounded-full bg-west-600 text-white font-medium hover:bg-west-700 transition-colors"
              >
                Try the Web Tool
              </button>
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
                className="flex items-center justify-center gap-2 w-full px-6 py-3 rounded-full bg-slate-200 text-slate-800 font-medium hover:bg-slate-300 transition-colors"
              >
                Formalization
                <ExternalLink className="w-4 h-4 ml-1" />
              </a>
            </div>
          </section>

          {/* Publications */}
          <section>
            <h2 className="text-2xl font-bold text-slate-900 mb-6">Publications</h2>
            <ol className="space-y-4 list-decimal list-outside ml-6">
              <li className="text-slate-800 leading-relaxed pl-2">
                Jenna Elwing, Laura Gamboa Guzman, Jeremy Sorkins, Chiara Travesset, Zili Wang, Kristin Rozier.{' '}
                <em>Mission-time LTL (MLTL) Formula Validation Via Regular Expressions</em>, International Conference on integrated Formal Methods (iFM), 2023 Proceedings, available{' '}
                <a href="https://link.springer.com/chapter/10.1007/978-3-031-47705-8_15" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  here
                </a>.
              </li>
              <li className="text-slate-800 leading-relaxed pl-2">
                Zili Wang, Laura P. Gamboa Guzman, Kristin Y. Rozier.{' '}
                <em>WEST: Interactive Validation of Mission-time Linear Temporal Logic (MLTL)</em>, to appear in Science of Computer Programming, 2025, available{' '}
                <a href="https://research.temporallogic.org/papers/WGR25.pdf" target="_blank" rel="noopener noreferrer" className="text-west-600 hover:text-west-700 underline underline-offset-2">
                  here
                </a>.
              </li>
              <li className="text-slate-800 leading-relaxed pl-2">
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
            <p className="text-slate-800 text-center">
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

      {/* ===== MLTL SYNTAX TAB ===== */}
      {activeTab === 'syntax' && (
        <div className="space-y-8">
          <section>
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-2xl font-bold text-slate-900">MLTL Syntax Reference</h2>
              <button
                onClick={() => navigate('tool')}
                className="flex items-center gap-1.5 text-sm text-west-600 hover:text-west-700 font-medium transition-colors"
              >
                <Play className="w-3.5 h-3.5" />
                Try the Tool
              </button>
            </div>
            <p className="text-slate-800 leading-relaxed mb-6">
              Mission-time Linear Temporal Logic (MLTL) extends propositional logic with bounded temporal operators. Below is the complete grammar supported by WEST.
            </p>
          </section>

          {/* Propositions */}
          <section className="bg-white rounded-xl shadow-md border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Propositions</h3>
            <p className="text-slate-800 mb-3">
              Propositions are alphanumeric identifiers that may include underscores. They represent atomic assertions.
            </p>
            <div className="bg-slate-50 rounded-lg p-4 font-mono text-sm">
              <code className="text-slate-800">p0, sensor_active, obstacle, Tired</code>
            </div>
          </section>

          {/* Boolean Constants */}
          <section className="bg-white rounded-xl shadow-md border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Boolean Constants</h3>
            <table className="w-full text-sm">
              <thead className="bg-slate-50">
                <tr>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tl-lg">Syntax</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tr-lg">Description</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-100">
                <tr><td className="px-4 py-2 font-mono text-west-700">true</td><td className="px-4 py-2 text-slate-800">Always true (case-insensitive)</td></tr>
                <tr><td className="px-4 py-2 font-mono text-west-700">false</td><td className="px-4 py-2 text-slate-800">Always false (case-insensitive)</td></tr>
              </tbody>
            </table>
          </section>

          {/* Boolean Operators */}
          <section className="bg-white rounded-xl shadow-md border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Boolean Operators</h3>
            <table className="w-full text-sm">
              <thead className="bg-slate-50">
                <tr>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tl-lg">Operator</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700">Syntax</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tr-lg">Example</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-100">
                <tr>
                  <td className="px-4 py-2 text-slate-800">Negation</td>
                  <td className="px-4 py-2 font-mono text-west-700">!, not</td>
                  <td className="px-4 py-2 font-mono text-slate-700">!p0, not active</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Conjunction (AND)</td>
                  <td className="px-4 py-2 font-mono text-west-700">&, &&, and</td>
                  <td className="px-4 py-2 font-mono text-slate-700">p0 & p1, x and y</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Disjunction (OR)</td>
                  <td className="px-4 py-2 font-mono text-west-700">|, ||, or</td>
                  <td className="px-4 py-2 font-mono text-slate-700">p0 | p1, x or y</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Implication</td>
                  <td className="px-4 py-2 font-mono text-west-700">-&gt;, implies</td>
                  <td className="px-4 py-2 font-mono text-slate-700">p0 -&gt; p1, rain implies wet</td>
                </tr>
              </tbody>
            </table>
            <p className="mt-4 text-sm text-slate-600">
              <strong>Precedence (highest to lowest):</strong> negation → conjunction → disjunction → implication
            </p>
          </section>

          {/* Temporal Operators */}
          <section className="bg-white rounded-xl shadow-md border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Temporal Operators</h3>
            <p className="text-slate-800 mb-4">
              Temporal operators require <strong>bounded intervals</strong> written as <code className="px-1.5 py-0.5 bg-slate-100 rounded font-mono text-sm">[a,b]</code> where <code className="font-mono">a ≤ b</code> are non-negative integers representing time bounds.
            </p>
            <table className="w-full text-sm">
              <thead className="bg-slate-50">
                <tr>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tl-lg">Operator</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700">Syntax</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700">Meaning</th>
                  <th className="text-left px-4 py-2 font-medium text-slate-700 rounded-tr-lg">Example</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-100">
                <tr>
                  <td className="px-4 py-2 text-slate-800">Globally</td>
                  <td className="px-4 py-2 font-mono text-west-700">G[a,b], global[a,b]</td>
                  <td className="px-4 py-2 text-slate-800">φ holds at all times in [a,b]</td>
                  <td className="px-4 py-2 font-mono text-slate-700">G[0,5](safe)</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Future</td>
                  <td className="px-4 py-2 font-mono text-west-700">F[a,b], future[a,b]</td>
                  <td className="px-4 py-2 text-slate-800">φ holds at some time in [a,b]</td>
                  <td className="px-4 py-2 font-mono text-slate-700">F[1,10](goal)</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Until</td>
                  <td className="px-4 py-2 font-mono text-west-700">U[a,b], until[a,b]</td>
                  <td className="px-4 py-2 text-slate-800">φ holds until ψ within [a,b]</td>
                  <td className="px-4 py-2 font-mono text-slate-700">wait U[0,3] go</td>
                </tr>
                <tr>
                  <td className="px-4 py-2 text-slate-800">Release</td>
                  <td className="px-4 py-2 font-mono text-west-700">R[a,b], release[a,b]</td>
                  <td className="px-4 py-2 text-slate-800">ψ holds until φ releases it</td>
                  <td className="px-4 py-2 font-mono text-slate-700">done R[0,5] hold</td>
                </tr>
              </tbody>
            </table>
          </section>

          {/* Example Formulas */}
          <section className="bg-white rounded-xl shadow-md border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Example Formulas</h3>
            <div className="space-y-3">
              <div className="bg-slate-50 rounded-lg p-4">
                <code className="font-mono text-sm text-west-700 block mb-1">G[0,10](obstacle -&gt; stop)</code>
                <p className="text-sm text-slate-600">Always within t=0 to t=10: if obstacle then stop</p>
              </div>
              <div className="bg-slate-50 rounded-lg p-4">
                <code className="font-mono text-sm text-west-700 block mb-1">F[0,5](goal & !error)</code>
                <p className="text-sm text-slate-600">Eventually within 5 steps: goal is reached and no error</p>
              </div>
              <div className="bg-slate-50 rounded-lg p-4">
                <code className="font-mono text-sm text-west-700 block mb-1">Tired U[0,8] Sleep</code>
                <p className="text-sm text-slate-600">Tired holds until Sleep occurs within 8 steps</p>
              </div>
              <div className="bg-slate-50 rounded-lg p-4">
                <code className="font-mono text-sm text-west-700 block mb-1">G[0,5](request -&gt; F[1,3](response))</code>
                <p className="text-sm text-slate-600">Always: every request gets a response within 1-3 steps</p>
              </div>
            </div>
          </section>

          {/* Notes */}
          <section className="bg-slate-50 rounded-xl border border-slate-200 p-6">
            <h3 className="text-lg font-bold text-slate-900 mb-3">Notes</h3>
            <ul className="list-disc list-inside space-y-2 text-slate-800">
              <li>All keywords are <strong>case-insensitive</strong>: <code className="font-mono text-sm">G</code>, <code className="font-mono text-sm">global</code>, <code className="font-mono text-sm">GLOBAL</code> are equivalent</li>
              <li>Use parentheses to override default precedence</li>
              <li>Time bounds must satisfy <code className="font-mono text-sm">a ≤ b</code> (e.g., <code className="font-mono text-sm">[0,5]</code> not <code className="font-mono text-sm">[5,0]</code>)</li>
              <li>Implication is <strong>right-associative</strong>: <code className="font-mono text-sm">a -&gt; b -&gt; c</code> means <code className="font-mono text-sm">a -&gt; (b -&gt; c)</code></li>
            </ul>
          </section>
        </div>
      )}

      {/* ===== TOOL TAB ===== */}
      {activeTab === 'tool' && (
        <>
        {/* Formula Input Section */}
        <section className="mb-8 space-y-3">
          <div className="bg-white rounded-2xl shadow-xl shadow-slate-200/50 border border-slate-200 overflow-hidden p-6">
                <div className="flex items-center gap-2 mb-2">
                  <label htmlFor="formula" className="block text-sm font-medium text-slate-900">
                    MLTL Formula
                  </label>
                  <button
                    onClick={() => navigate('syntax')}
                    className="flex items-center gap-1 text-xs text-slate-500 hover:text-west-600 transition-colors"
                    title="View MLTL syntax reference"
                  >
                    <HelpCircle className="w-3.5 h-3.5" />
                    <span>Syntax Help</span>
                  </button>
                </div>
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
                <p className="mt-2 text-xs text-slate-600">
                  Press <kbd className="px-1.5 py-0.5 rounded bg-slate-100 font-mono text-[10px]">Ctrl</kbd>+<kbd className="px-1.5 py-0.5 rounded bg-slate-100 font-mono text-[10px]">Enter</kbd> to run
                </p>

                {/* NNF — collapsible after run */}
                {result && result.nnf && (
                  <div className="mt-3 pt-3 border-t border-slate-100">
                    <button
                      onClick={() => setNnfOpen((prev) => !prev)}
                      className="flex items-center gap-1.5 text-xs font-medium text-slate-500 hover:text-slate-700 transition-colors"
                    >
                      {nnfOpen ? <ChevronUp className="w-3.5 h-3.5" /> : <ChevronDown className="w-3.5 h-3.5" />}
                      Negation Normal Form
                    </button>
                    {nnfOpen && (
                      <div className="flex items-start justify-between gap-3 mt-2">
                        <code className="font-mono text-sm text-west-700 break-all">{result.nnf}</code>
                        <button
                          onClick={() => copyToClipboard(result.nnf)}
                          className="shrink-0 text-slate-700 hover:text-slate-900 transition-colors"
                          title="Copy NNF"
                        >
                          {copied ? <CheckCircle2 className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                        </button>
                      </div>
                    )}
                  </div>
                )}
          </div>

          {/* Collapsible examples section below input */}
          <div className="bg-white rounded-2xl shadow-md shadow-slate-200/50 border border-slate-200 overflow-hidden">
            <button
              onClick={() => setExamplesOpen((prev) => !prev)}
              className="flex items-center justify-between w-full px-5 py-3 hover:bg-slate-50 transition-colors"
            >
              <span className="text-xs font-bold text-slate-900 uppercase tracking-widest">Example Formulas</span>
              {examplesOpen ? <ChevronUp className="w-4 h-4 text-slate-700" /> : <ChevronDown className="w-4 h-4 text-slate-700" />}
            </button>
            {examplesOpen && (
              <div className="px-5 pb-4 max-h-64 overflow-y-auto border-t border-slate-100" style={{ scrollbarWidth: 'thin' }}>
                <div className="grid sm:grid-cols-2 gap-2 pt-3">
                  {exampleFormulas.map((ex) => (
                    <button
                      key={ex.formula}
                      onClick={() => setFormula(ex.formula)}
                      className="w-full text-left px-3 py-2.5 rounded-xl bg-slate-50 border border-slate-200 hover:border-west-300 hover:bg-west-50 transition-colors group"
                    >
                      <p className="text-xs font-semibold text-slate-800 group-hover:text-west-700 transition-colors">{ex.label}</p>
                      <code className="text-[10px] font-mono text-slate-600 group-hover:text-west-500 break-all transition-colors">{ex.formula}</code>
                    </button>
                  ))}
                </div>
              </div>
            )}
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
              <div ref={subformulasRef} className="bg-white rounded-2xl shadow-xl shadow-slate-200/50 border border-slate-200 overflow-hidden">
                <div className="flex min-h-0">
                  {/* Left: vertical subformula selector (collapsible) */}
                  {subfListOpen && (
                  <div className="w-56 shrink-0 border-r border-slate-100 bg-slate-50 flex flex-col">
                    <div className="flex items-center justify-between px-4 pt-4 pb-2">
                      <p className="text-xs font-bold text-slate-900 uppercase tracking-widest">Subformulas</p>
                      <button
                        onClick={() => setSubfListOpen(false)}
                        className="text-slate-400 hover:text-slate-600 transition-colors"
                        title="Hide subformula list"
                      >
                        <ChevronLeft className="w-4 h-4" />
                      </button>
                    </div>
                    <div className="flex-1 overflow-y-auto px-3 pb-3 space-y-1.5" style={{ scrollbarWidth: 'thin', maxHeight: '70vh' }}>
                      {result.subformulas.map((sf, i) => (
                        <button
                          key={i}
                          onClick={() => setSelectedSubf(i)}
                          className={`w-full px-3 py-2.5 rounded-xl border-2 text-left transition-all duration-150 ${
                            selectedSubf === i
                              ? 'border-west-500 bg-west-50 shadow-md shadow-west-100/60'
                              : 'border-slate-200 bg-white hover:border-west-300 hover:bg-west-50/50'
                          }`}
                        >
                          <div className={`font-mono text-xs font-semibold ${
                            selectedSubf === i ? 'text-west-800' : 'text-slate-800'
                          }`}>{sf.formula}</div>
                          <div className={`text-[10px] mt-0.5 ${
                            selectedSubf === i ? 'text-west-500' : 'text-slate-600'
                          }`}>{sf.count.toLocaleString()} patterns</div>
                        </button>
                      ))}
                    </div>
                  </div>
                  )}

                  {/* Right: trace table for selected subformula */}
                  <div className="flex-1 min-w-0">
                    {!subfListOpen && (
                      <div className="px-5 pt-4 pb-0">
                        <button
                          onClick={() => setSubfListOpen(true)}
                          className="flex items-center gap-1 text-xs text-slate-500 hover:text-slate-700 font-medium transition-colors"
                        >
                          <ChevronRight className="w-3.5 h-3.5" />
                          Show subformulas
                        </button>
                      </div>
                    )}
                    <TraceTable
                      sf={result.subformulas[selectedSubf] ?? result.subformulas[0]}
                      n={result.prop_vars.length}
                      t={(result.subformulas[selectedSubf] ?? result.subformulas[0])?.complen ?? result.computation_length}
                      varNames={result.prop_vars}
                      resultKey={result.nnf}
                      currentFormula={result.subformulas[selectedSubf]?.formula ?? result.subformulas[0]?.formula}
                      unsatComputations={(result.subformulas[selectedSubf] ?? result.subformulas[0])?.unsat_computations ?? []}
                      mentionedVars={(result.subformulas[selectedSubf] ?? result.subformulas[0])?.mentioned_vars}
                    />
                  </div>
                </div>
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
                  className="flex items-center gap-2 px-4 py-2 rounded-lg bg-slate-100 hover:bg-slate-200 text-slate-800 text-sm font-medium transition-colors"
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
        <div className="max-w-screen-2xl mx-auto px-4 sm:px-6 lg:px-10 py-6">
          <p className="text-center text-sm text-slate-700">
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

// Consistent color palette for variable-indexed bit positions
const VAR_COLORS = [
  { text: 'text-sky-400',    dot: 'bg-sky-400' },
  { text: 'text-amber-400',  dot: 'bg-amber-400' },
  { text: 'text-emerald-400', dot: 'bg-emerald-400' },
  { text: 'text-pink-400',   dot: 'bg-pink-400' },
  { text: 'text-violet-400', dot: 'bg-violet-400' },
  { text: 'text-orange-400', dot: 'bg-orange-400' },
  { text: 'text-teal-400',   dot: 'bg-teal-400' },
  { text: 'text-rose-400',   dot: 'bg-rose-400' },
]

function TraceTable({ sf, n, t, varNames, resultKey, currentFormula, unsatComputations = [], mentionedVars }) {
  const [toggle, setToggle] = useState(() =>
    Array.from({ length: n }, () => Array(t).fill(false))
  )

  // Reset toggle whenever the formula/subformula changes or dimensions change
  useEffect(() => {
    setToggle(Array.from({ length: n }, () => Array(t).fill(false)))
  }, [resultKey, n, t, currentFormula])

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
    <p className="text-xs text-slate-600 italic px-4 py-2">No variables to display.</p>
  )

  return (
    <div className="p-5 space-y-5">
      {/* Current formula display with SAT/UNSAT badge */}
      {currentFormula && (
        <div className="rounded-lg bg-slate-50 border border-slate-200 px-4 py-2.5">
          <div className="flex items-center gap-2 mb-1">
            <p className="text-xs font-bold text-slate-900 uppercase tracking-widest">Current Formula</p>
            <span className={`px-2 py-0.5 rounded-md text-[10px] font-bold uppercase tracking-wide ${
              isSat
                ? 'bg-green-100 text-green-700 border border-green-300'
                : 'bg-red-100 text-red-700 border border-red-300'
            }`}>
              {isSat ? 'satisfied' : 'not satisfied'}
            </span>
          </div>
          <code className="font-mono text-sm text-west-700 break-all">{currentFormula}</code>
        </div>
      )}

      {/* Trace string — annotated display */}
      <div className="rounded-xl bg-slate-900 px-4 py-3">
        <p className="text-[10px] font-semibold text-slate-300 uppercase tracking-widest mb-2">Current Trace</p>
        {/* Variable index legend */}
        <div className="flex items-center gap-3 mb-2 flex-wrap">
          {varNames.map((name, vi) => {
            const isRelevant = !mentionedVars || mentionedVars.includes(vi)
            return (
              <span key={vi} className={`flex items-center gap-1 ${isRelevant ? '' : 'opacity-30'}`}>
                <span className={`inline-block w-2 h-2 rounded-sm ${VAR_COLORS[vi % VAR_COLORS.length].dot}`} />
                <span className="text-[10px] font-mono text-slate-400">
                  bit {vi} = <span className="text-slate-200">{name}</span>
                </span>
              </span>
            )
          })}
        </div>
        {/* Annotated trace: color-coded bits per variable */}
        <div className="flex items-center gap-0 flex-wrap font-mono text-base leading-relaxed">
          {Array.from({ length: t }, (_, time) => {
            const chunk = Array.from({ length: n }, (_, v) => toggle[v]?.[time] ? '1' : '0')
            return (
              <span key={time} className="inline-flex items-center">
                {time > 0 && <span className="text-slate-500 mx-0.5">,</span>}
                {chunk.map((bit, vi) => {
                  const isRelevant = !mentionedVars || mentionedVars.includes(vi)
                  return (
                    <span key={vi} className={`${VAR_COLORS[vi % VAR_COLORS.length].text} font-semibold ${isRelevant ? '' : 'opacity-30'}`}>
                      {bit}
                    </span>
                  )
                })}
              </span>
            )
          })}
        </div>
      </div>

      {/* Interactive table */}
      <div>
        <div className="flex items-center gap-3 mb-2">
          <p className="text-xs font-bold text-slate-900 uppercase tracking-widest">Interactive Trace Editor</p>
          <button
            onClick={reset}
            className="text-xs px-2.5 py-1 bg-transparent border border-slate-200 rounded-lg hover:bg-slate-100 hover:border-slate-300 text-slate-400 hover:text-slate-600 font-medium transition-colors"
          >
            Reset Trace
          </button>
        </div>
        <div className="overflow-x-auto rounded-xl border border-slate-200">
          <table className="text-xs border-collapse w-full">
            <thead className="bg-slate-50">
              <tr>
                <th className="px-3 py-2 text-left text-slate-700 font-medium border-b border-slate-200">Variable</th>
                {Array.from({ length: t }, (_, time) => (
                  <th key={time} className="px-3 py-2 text-center text-slate-700 font-medium min-w-[3.5rem] border-b border-slate-200">
                    t={time}
                  </th>
                ))}
              </tr>
            </thead>
            <tbody>
              {Array.from({ length: n }, (_, v) => {
                const isRelevant = !mentionedVars || mentionedVars.includes(v)
                return (
                  <tr key={v} className={`border-b border-slate-100 last:border-0 transition-colors ${
                    isRelevant ? 'hover:bg-slate-50' : 'bg-slate-50/50'
                  }`}>
                    <td className={`px-3 py-2 font-mono font-medium whitespace-nowrap ${isRelevant ? 'bg-white' : 'bg-slate-50/50'}`}>
                      <span className="flex items-center gap-1.5">
                        <span className={`inline-block w-2 h-2 rounded-sm shrink-0 ${VAR_COLORS[v % VAR_COLORS.length].dot} ${isRelevant ? '' : 'opacity-30'}`} />
                        <span className={isRelevant ? 'text-slate-800' : 'text-slate-400 italic'}>{varNames[v] ?? `p${v}`}</span>
                      </span>
                    </td>
                    {Array.from({ length: t }, (_, time) => (
                      <td key={time} className={`px-3 py-2 text-center ${isRelevant ? '' : 'opacity-30'}`}>
                        <input
                          type="checkbox"
                          checked={toggle[v]?.[time] ?? false}
                          onChange={(e) => setCell(v, time, e.target.checked)}
                          className="w-4 h-4 cursor-pointer accent-west-600"
                        />
                      </td>
                    ))}
                  </tr>
                )
              })}
            </tbody>
          </table>
        </div>
      </div>

      {/* Random SAT / UNSAT buttons */}
      <div className="flex items-center gap-2">
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

      {/* Computed regex patterns */}
      <div>
        <div className="flex items-end justify-between mb-2">
          <p className="text-xs font-bold text-slate-900 uppercase tracking-widest">
            Computed Regex Patterns ({sf.count.toLocaleString()})
          </p>
          <span className="flex items-center gap-1.5 text-[10px] text-slate-600 italic">
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
                        : 'bg-slate-100 text-slate-600 hover:bg-west-100 hover:text-west-700'
                    }`}
                  >
                    <Shuffle className="w-2.5 h-2.5" />
                    use
                  </button>
                  <span
                    title="Click 'use' to randomly resolve this pattern's wildcards and load it into the trace editor"
                    className="text-[11px] text-slate-500 hover:text-slate-800 cursor-help select-none px-0.5"
                  >?</span>
                </div>
                <code className={`flex-1 min-w-0 font-mono text-sm break-all ${
                  isMatch ? 'text-green-800 font-semibold' : 'text-slate-700'
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
