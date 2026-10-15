# Authenticated server-side planning — 2026-10-15

The viewer can use the world server's AI without installing a local model. The
native `FinalverseWorld` capability advertises `server_planning: true` in its
session-bound `experience` result. Create routes prompts and replayed WorldSpecs
through that capability; legacy local fixtures retain loopback planning.

```mermaid
sequenceDiagram
    participant User
    participant Viewer
    participant MutSea
    participant Gateway
    User->>Viewer: Ask or create
    Viewer->>MutSea: Authenticated plan(prompt or recipe)
    MutSea->>MutSea: Check active root session; inspect scoped scene
    MutSea->>Gateway: Private Unix-socket proposal request
    Gateway-->>MutSea: Validated proposal; no execution
    MutSea-->>Viewer: Answer and WorldPlan
    Viewer->>MutSea: Prepare proposal
    MutSea-->>Viewer: Native permission/geometry preview
    User->>Viewer: Approve
    Viewer->>MutSea: Commit exact prepared plan
    MutSea->>MutSea: Execute; persist WorldLine
    MutSea-->>Viewer: Observable result and Undo
```

Native source is `Finalverse/World/PlanningBridge.cs` and `ServerPlanner.cs` in
the MutSea repository. Prompts are limited to 2048 characters. The client cannot
supply actor IDs, provider endpoints or world context. The server projects its
own 64-meter inspection into a bounded context excluding credentials, capability
URLs, inventory, memories and ownership metadata. Gateway scoping further limits
model observations to twelve relevant entities. Scene labels remain untrusted
data. Recipes retain catalog/solver pins and carry no coordinates or authority.

Inference releases the transaction gate. Admission permits one pending inference
per actor, six requests per minute per actor and two simultaneous requests in the
process. Requests and responses have byte/depth bounds and a 45-second transport
deadline. An expired or changed region session cannot receive the late result.
Prepare and Commit retain their existing validation, approval expiry and guarded
Undo; planning never executes an action.

The gateway deployment uses Linux peer credentials, a socket with mode 0600 and
an owned directory with mode 0700, shared with the simulator UID. Its directory
survives gateway restarts so the container mount stays valid. Model inference is
local to the backend today. Future cloud adapters must keep privileged secrets
server-side and preserve the same proposal-only contract.

The viewer waits for initial discovery before choosing a planner route and treats
an incomplete world response as a session error. The inherited HTTP server can
return an empty HTTP 200 for a capability route removed at logout; that empty
response is not a valid world result.

## Verification

On the private pilot deployment, a real remote client used validated TLS for
login and capabilities, asked about nearby entities, approved one blue primitive,
received it over region UDP, retained it through native TLS rotation/restart, and
undid it over UDP. WorldLine retained the committed operation. A forged context
was rejected. A duplicate inference was rejected while native inspection returned
in 326.5 ms during the first inference. The removed logout capability yielded no
operation. Observation took approximately 12.2 seconds and creation planning
25.5 seconds on the backend's local model; these are individual measurements.

Relevant automated checks: 56 World Kernel checks, 12 planning boundary checks,
74 gateway tests on Linux, and the viewer presentation/recipe tests. The updated
Mac runtime also exposed an inherited null-key XML comparator ordering defect;
the fix restores strict weak ordering and has a native regression test.

The universal Mac app has entered the actual remote Harbor and displayed a
server-generated nearby answer (Plaza Paver, Path Lantern and Plaza Compass).
The preview review exposed an existing rendering hazard: a network continuation
could upload a missing font glyph while setting activity text, acquiring a
non-coroutine-aware GL mutex. Activity layout and control refresh now run in the
main draw loop; network continuations update presentation state. World-operation
HTTP requests also disable automatic retries, preventing a dropped response from
replaying a mutation. The corrected universal app builds successfully and its
three focused test suites pass.

The rebuilt Mac app then completed the real remote interaction: Ask AI generated
one `Native UI Test Marker` proposal in 27.9 seconds; the native preview showed a
single 0.6-meter box and enabled Apply only after placement/permission checks.
Explicit Apply returned the committed result, the marker arrived in the viewer,
and History displayed its intent and committed WorldLine entries. Undo returned
“Undone. The previous world state was restored.” PostgreSQL independently
confirmed 422 → 423 → 422 parts and marker count 0 → 1 → 0. The preview, History
and Undo text rendered without the previous font/GL coroutine assertion.
Private review evidence is retained outside Git in the development verification
bundle (`mac-ui-committed.png`, `mac-ui-history.png`, `mac-ui-undone.png`). This is
a functional approval/undo check, not a claim of directional-placement quality
or a premium-art/performance review.

The private review app contains preserved unrelated viewer work and is not a
distribution artifact. Public signup and unrestricted grid access remain separate
release work. Voice and the remote Lumi agent runtime were not exercised.
